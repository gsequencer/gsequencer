/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2021 Joël Krähemann
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

int ags_resample_util_test_init_suite();
int ags_resample_util_test_clean_suite();

void ags_resample_util_test_alloc();
void ags_resample_util_test_copy();
void ags_resample_util_test_free();
void ags_resample_util_test_get_destination();
void ags_resample_util_test_set_destination();
void ags_resample_util_test_get_destination_stride();
void ags_resample_util_test_set_destination_stride();
void ags_resample_util_test_get_source();
void ags_resample_util_test_set_source();
void ags_resample_util_test_get_source_stride();
void ags_resample_util_test_set_source_stride();
void ags_resample_util_test_get_buffer_length();
void ags_resample_util_test_set_buffer_length();
void ags_resample_util_test_get_audio_buffer_util_format();
void ags_resample_util_test_set_audio_buffer_util_format();
void ags_resample_util_test_get_samplerate();
void ags_resample_util_test_set_samplerate();
void ags_resample_util_test_get_target_samplerate();
void ags_resample_util_test_set_target_samplerate();
void ags_resample_util_test_compute_s8();
void ags_resample_util_test_compute_s16();
void ags_resample_util_test_compute_s24();
void ags_resample_util_test_compute_s32();
void ags_resample_util_test_compute_s64();
void ags_resample_util_test_compute_float();
void ags_resample_util_test_compute_double();
void ags_resample_util_test_compute_complex();
void ags_resample_util_test_compute();

#define AGS_RESAMPLE_UTIL_TEST_COPY_SOURCE_AUDIO_CHANNELS (2)
#define AGS_RESAMPLE_UTIL_TEST_COPY_BUFFER_SIZE (1024)
#define AGS_RESAMPLE_UTIL_TEST_COPY_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)
#define AGS_RESAMPLE_UTIL_TEST_COPY_SAMPLERATE (48000)
#define AGS_RESAMPLE_UTIL_TEST_COPY_TARGET_SAMPLERATE (44100)

#define AGS_RESAMPLE_UTIL_TEST_GET_DESTINATION_BUFFER_SIZE (1024)
#define AGS_RESAMPLE_UTIL_TEST_GET_DESTINATION_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

#define AGS_RESAMPLE_UTIL_TEST_SET_DESTINATION_BUFFER_SIZE (1024)
#define AGS_RESAMPLE_UTIL_TEST_SET_DESTINATION_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

#define AGS_RESAMPLE_UTIL_TEST_GET_SOURCE_BUFFER_SIZE (1024)
#define AGS_RESAMPLE_UTIL_TEST_GET_SOURCE_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

#define AGS_RESAMPLE_UTIL_TEST_SET_SOURCE_BUFFER_SIZE (1024)
#define AGS_RESAMPLE_UTIL_TEST_SET_SOURCE_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_resample_util_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_resample_util_test_clean_suite()
{
  return(0);
}

void
ags_resample_util_test_alloc()
{
  AgsResampleUtil *resample_util;

  resample_util = ags_resample_util_alloc();

  CU_ASSERT(resample_util != NULL);

  CU_ASSERT(resample_util->destination == NULL);
  CU_ASSERT(resample_util->destination_stride == 1);

  CU_ASSERT(resample_util->source == NULL);
  CU_ASSERT(resample_util->source_stride == 1);
  
  CU_ASSERT(resample_util->buffer_length == 0);
  CU_ASSERT(resample_util->audio_buffer_util_format == AGS_RESAMPLE_UTIL_DEFAULT_AUDIO_BUFFER_UTIL_FORMAT);
  CU_ASSERT(resample_util->samplerate == AGS_RESAMPLE_UTIL_DEFAULT_SAMPLERATE);
  
  CU_ASSERT(resample_util->target_samplerate == AGS_RESAMPLE_UTIL_DEFAULT_TARGET_SAMPLERATE);
}

void
ags_resample_util_test_copy()
{
  AgsResampleUtil resample_util;
  AgsResampleUtil *copy_resample_util;

  resample_util = (AgsResampleUtil) {
    .destination = ags_stream_alloc(AGS_RESAMPLE_UTIL_TEST_COPY_BUFFER_SIZE,
				    AGS_RESAMPLE_UTIL_TEST_COPY_FORMAT),
    .source = ags_stream_alloc(AGS_RESAMPLE_UTIL_TEST_COPY_SOURCE_AUDIO_CHANNELS * AGS_RESAMPLE_UTIL_TEST_COPY_BUFFER_SIZE,
			       AGS_RESAMPLE_UTIL_TEST_COPY_FORMAT),
    .source_stride = AGS_RESAMPLE_UTIL_TEST_COPY_SOURCE_AUDIO_CHANNELS,
    .buffer_length = AGS_RESAMPLE_UTIL_TEST_COPY_BUFFER_SIZE,
    .samplerate = AGS_RESAMPLE_UTIL_TEST_COPY_SAMPLERATE,
    .target_samplerate = AGS_RESAMPLE_UTIL_TEST_COPY_TARGET_SAMPLERATE
  };

  copy_resample_util = ags_resample_util_copy(&resample_util);
  
  CU_ASSERT(copy_resample_util != NULL);
  
  CU_ASSERT(copy_resample_util->destination == resample_util.destination);
  CU_ASSERT(copy_resample_util->destination_stride == resample_util.destination_stride);
  
  CU_ASSERT(copy_resample_util->source == resample_util.source);
  CU_ASSERT(copy_resample_util->source_stride == resample_util.source_stride);
  
  CU_ASSERT(copy_resample_util->buffer_length == resample_util.buffer_length);
  CU_ASSERT(copy_resample_util->audio_buffer_util_format == resample_util.audio_buffer_util_format);
  CU_ASSERT(copy_resample_util->samplerate == resample_util.samplerate);

  CU_ASSERT(copy_resample_util->target_samplerate == resample_util.target_samplerate);
}

void
ags_resample_util_test_free()
{
  AgsResampleUtil *resample_util;
  
  resample_util = (AgsResampleUtil *) g_new(AgsResampleUtil,
					    1);

  CU_ASSERT(resample_util != NULL);

  resample_util->source = NULL;
  resample_util->destination = NULL;
  
  resample_util->secret_rabbit.data_out = NULL;
  resample_util->secret_rabbit.data_in = NULL;

  ags_resample_util_free(resample_util);
  
  resample_util = ags_resample_util_alloc();

  CU_ASSERT(resample_util != NULL);

  ags_resample_util_free(resample_util);
}

void
ags_resample_util_test_get_destination()
{
  AgsResampleUtil resample_util;

  resample_util.destination = NULL;

  CU_ASSERT(ags_resample_util_get_destination(&resample_util) == NULL);
  
  resample_util.destination = ags_stream_alloc(AGS_RESAMPLE_UTIL_TEST_GET_DESTINATION_BUFFER_SIZE,
					       AGS_RESAMPLE_UTIL_TEST_GET_DESTINATION_FORMAT);

  CU_ASSERT(ags_resample_util_get_destination(&resample_util) == resample_util.destination);
}

void
ags_resample_util_test_set_destination()
{
  AgsResampleUtil resample_util;

  gpointer destination;
  
  resample_util.destination = NULL;

  destination = ags_stream_alloc(AGS_RESAMPLE_UTIL_TEST_SET_DESTINATION_BUFFER_SIZE,
				 AGS_RESAMPLE_UTIL_TEST_SET_DESTINATION_FORMAT);

  ags_resample_util_set_destination(&resample_util,
				    destination);
  
  CU_ASSERT(resample_util.destination == destination);
}

void
ags_resample_util_test_get_destination_stride()
{
  AgsResampleUtil resample_util;

  resample_util.destination_stride = 0;

  CU_ASSERT(ags_resample_util_get_destination_stride(&resample_util) == 0);
  
  resample_util.destination_stride = 1;

  CU_ASSERT(ags_resample_util_get_destination_stride(&resample_util) == resample_util.destination_stride);
}

void
ags_resample_util_test_set_destination_stride()
{
  AgsResampleUtil resample_util;
  
  resample_util.destination_stride = 0;

  ags_resample_util_set_destination_stride(&resample_util,
					   1);
  
  CU_ASSERT(resample_util.destination_stride == 1);
}

void
ags_resample_util_test_get_source()
{
  AgsResampleUtil resample_util;

  resample_util.source = NULL;

  CU_ASSERT(ags_resample_util_get_source(&resample_util) == NULL);
  
  resample_util.source = ags_stream_alloc(AGS_RESAMPLE_UTIL_TEST_GET_SOURCE_BUFFER_SIZE,
					  AGS_RESAMPLE_UTIL_TEST_GET_SOURCE_FORMAT);

  CU_ASSERT(ags_resample_util_get_source(&resample_util) == resample_util.source);
}

void
ags_resample_util_test_set_source()
{
  AgsResampleUtil resample_util;

  gpointer source;
  
  resample_util.source = NULL;

  source = ags_stream_alloc(AGS_RESAMPLE_UTIL_TEST_SET_SOURCE_BUFFER_SIZE,
			    AGS_RESAMPLE_UTIL_TEST_SET_SOURCE_FORMAT);
  
  ags_resample_util_set_source(&resample_util,
			       source);
  
  CU_ASSERT(resample_util.source == source);
}

void
ags_resample_util_test_get_source_stride()
{
  AgsResampleUtil resample_util;

  resample_util.source_stride = 0;

  CU_ASSERT(ags_resample_util_get_source_stride(&resample_util) == 0);
  
  resample_util.source_stride = 1;

  CU_ASSERT(ags_resample_util_get_source_stride(&resample_util) == resample_util.source_stride);
}

void
ags_resample_util_test_set_source_stride()
{
  AgsResampleUtil resample_util;
  
  resample_util.source_stride = 0;

  ags_resample_util_set_source_stride(&resample_util,
				      1);
  
  CU_ASSERT(resample_util.source_stride == 1);
}

void
ags_resample_util_test_get_buffer_length()
{
  AgsResampleUtil resample_util;

  resample_util.buffer_length = 0;

  CU_ASSERT(ags_resample_util_get_buffer_length(&resample_util) == 0);
  
  resample_util.buffer_length = 512;

  CU_ASSERT(ags_resample_util_get_buffer_length(&resample_util) == resample_util.buffer_length);
}

void
ags_resample_util_test_set_buffer_length()
{
  AgsResampleUtil resample_util;

  resample_util.secret_rabbit.data_in = NULL;
  resample_util.secret_rabbit.data_out = NULL;
  
  resample_util.secret_rabbit.src_ratio = 1.0;

  resample_util.buffer_length = 0;
  
  ags_resample_util_set_buffer_length(&resample_util,
				      512);
  
  CU_ASSERT(resample_util.buffer_length == 512);
}

void
ags_resample_util_test_get_audio_buffer_util_format()
{
  AgsResampleUtil resample_util;

  resample_util.audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S16;

  CU_ASSERT(ags_resample_util_get_audio_buffer_util_format(&resample_util) == AGS_AUDIO_BUFFER_UTIL_S16);
  
  resample_util.audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_FLOAT;

  CU_ASSERT(ags_resample_util_get_audio_buffer_util_format(&resample_util) == resample_util.audio_buffer_util_format);
}

void
ags_resample_util_test_set_audio_buffer_util_format()
{
  AgsResampleUtil resample_util;
  
  resample_util.audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S16;

  ags_resample_util_set_audio_buffer_util_format(&resample_util,
						 AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
  CU_ASSERT(resample_util.audio_buffer_util_format == AGS_AUDIO_BUFFER_UTIL_FLOAT);
}

void
ags_resample_util_test_get_samplerate()
{
  AgsResampleUtil resample_util;

  resample_util.samplerate = 0;

  CU_ASSERT(ags_resample_util_get_samplerate(&resample_util) == 0);
  
  resample_util.samplerate = 44100;

  CU_ASSERT(ags_resample_util_get_samplerate(&resample_util) == resample_util.samplerate);
}

void
ags_resample_util_test_set_samplerate()
{
  AgsResampleUtil resample_util;
  
  resample_util.secret_rabbit.data_out = NULL;

  resample_util.secret_rabbit.src_ratio = 1.0;

  resample_util.buffer_length = 0;
  resample_util.samplerate = 0;
  resample_util.target_samplerate = 44100;

  ags_resample_util_set_samplerate(&resample_util,
				   44100);
  
  CU_ASSERT(resample_util.samplerate == 44100);
}

void
ags_resample_util_test_get_target_samplerate()
{
  AgsResampleUtil resample_util;

  resample_util.target_samplerate = 0;

  CU_ASSERT(ags_resample_util_get_target_samplerate(&resample_util) == 0);
  
  resample_util.target_samplerate = 44100;

  CU_ASSERT(ags_resample_util_get_target_samplerate(&resample_util) == resample_util.target_samplerate);
}

void
ags_resample_util_test_set_target_samplerate()
{
  AgsResampleUtil resample_util;
  
  resample_util.secret_rabbit.data_out = NULL;

  resample_util.secret_rabbit.src_ratio = 1.0;

  resample_util.buffer_length = 0;
  resample_util.samplerate = 44100;
  resample_util.target_samplerate = 0;

  ags_resample_util_set_target_samplerate(&resample_util,
					  44100);
  
  CU_ASSERT(resample_util.target_samplerate == 44100);
}

void
ags_resample_util_test_compute_s8()
{
  AgsResampleUtil resample_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  resample_util = (AgsResampleUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S8,
    .samplerate = 48000,
    .target_samplerate = 44100
  };
  
  ags_resample_util_compute_s8(&resample_util);
}

void
ags_resample_util_test_compute_s16()
{
  AgsResampleUtil resample_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  resample_util = (AgsResampleUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S16,
    .samplerate = 48000,
    .target_samplerate = 44100
  };
  
  ags_resample_util_compute_s16(&resample_util);
}

void
ags_resample_util_test_compute_s24()
{
  AgsResampleUtil resample_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  resample_util = (AgsResampleUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S24,
    .samplerate = 48000,
    .target_samplerate = 44100
  };
  
  ags_resample_util_compute_s24(&resample_util);
}

void
ags_resample_util_test_compute_s32()
{
  AgsResampleUtil resample_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  resample_util = (AgsResampleUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S32,
    .samplerate = 48000,
    .target_samplerate = 44100
  };
  
  ags_resample_util_compute_s32(&resample_util);
}

void
ags_resample_util_test_compute_s64()
{
  AgsResampleUtil resample_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  resample_util = (AgsResampleUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S64,
    .samplerate = 48000,
    .target_samplerate = 44100
  };
  
  ags_resample_util_compute_s64(&resample_util);
}

void
ags_resample_util_test_compute_float()
{
  AgsResampleUtil resample_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_FLOAT);

  resample_util = (AgsResampleUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_FLOAT,
    .samplerate = 48000,
    .target_samplerate = 44100
  };
  
  ags_resample_util_compute_float(&resample_util);
}

void
ags_resample_util_test_compute_double()
{
  AgsResampleUtil resample_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_DOUBLE);

  resample_util = (AgsResampleUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_DOUBLE,
    .samplerate = 48000,
    .target_samplerate = 44100
  };
  
  ags_resample_util_compute_double(&resample_util);
}

void
ags_resample_util_test_compute_complex()
{
  AgsResampleUtil resample_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_COMPLEX);

  resample_util = (AgsResampleUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_COMPLEX,
    .samplerate = 48000,
    .target_samplerate = 44100
  };
  
  ags_resample_util_compute_complex(&resample_util);
}

void
ags_resample_util_test_compute()
{
  AgsResampleUtil resample_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  resample_util = (AgsResampleUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S16,
    .samplerate = 48000,
    .target_samplerate = 44100
  };
  
  ags_resample_util_compute(&resample_util);
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
  pSuite = CU_add_suite("AgsResampleUtilTest", ags_resample_util_test_init_suite, ags_resample_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsResampleUtil alloc", ags_resample_util_test_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil copy", ags_resample_util_test_copy) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil free", ags_resample_util_test_free) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil get destination", ags_resample_util_test_get_destination) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil set destination", ags_resample_util_test_set_destination) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil get destination stride", ags_resample_util_test_get_destination_stride) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil set destination stride", ags_resample_util_test_set_destination_stride) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil get source", ags_resample_util_test_get_source) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil set source", ags_resample_util_test_set_source) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil get source stride", ags_resample_util_test_get_source_stride) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil set source stride", ags_resample_util_test_set_source_stride) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil get buffer length", ags_resample_util_test_get_buffer_length) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil set buffer length", ags_resample_util_test_set_buffer_length) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil get audio buffer util format", ags_resample_util_test_get_audio_buffer_util_format) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil set audio buffer util format", ags_resample_util_test_set_audio_buffer_util_format) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil get samplerate", ags_resample_util_test_get_samplerate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil set samplerate", ags_resample_util_test_set_samplerate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil get target samplerate", ags_resample_util_test_get_target_samplerate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil set target samplerate", ags_resample_util_test_set_target_samplerate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil compute s8", ags_resample_util_test_compute_s8) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil compute s16", ags_resample_util_test_compute_s16) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil compute s24", ags_resample_util_test_compute_s24) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil compute s32", ags_resample_util_test_compute_s32) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil compute s64", ags_resample_util_test_compute_s64) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil compute float", ags_resample_util_test_compute_float) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil compute double", ags_resample_util_test_compute_double) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil compute complex", ags_resample_util_test_compute_complex) == NULL) ||
     (CU_add_test(pSuite, "test of AgsResampleUtil compute", ags_resample_util_test_compute) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
