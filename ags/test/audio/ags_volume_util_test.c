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

int ags_volume_util_test_init_suite();
int ags_volume_util_test_clean_suite();

void ags_volume_util_test_alloc();
void ags_volume_util_test_copy();
void ags_volume_util_test_free();
void ags_volume_util_test_get_destination();
void ags_volume_util_test_set_destination();
void ags_volume_util_test_get_destination_stride();
void ags_volume_util_test_set_destination_stride();
void ags_volume_util_test_get_source();
void ags_volume_util_test_set_source();
void ags_volume_util_test_get_source_stride();
void ags_volume_util_test_set_source_stride();
void ags_volume_util_test_get_buffer_length();
void ags_volume_util_test_set_buffer_length();
void ags_volume_util_test_get_audio_buffer_util_format();
void ags_volume_util_test_set_audio_buffer_util_format();
void ags_volume_util_test_get_volume();
void ags_volume_util_test_set_volume();
void ags_volume_util_test_compute_s8();
void ags_volume_util_test_compute_s16();
void ags_volume_util_test_compute_s24();
void ags_volume_util_test_compute_s32();
void ags_volume_util_test_compute_s64();
void ags_volume_util_test_compute_float();
void ags_volume_util_test_compute_double();
void ags_volume_util_test_compute_complex();
void ags_volume_util_test_compute();

#define AGS_VOLUME_UTIL_TEST_COPY_BUFFER_SIZE (1024)
#define AGS_VOLUME_UTIL_TEST_COPY_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)
#define AGS_VOLUME_UTIL_TEST_COPY_SOURCE_AUDIO_CHANNELS (2)

#define AGS_VOLUME_UTIL_TEST_GET_DESTINATION_BUFFER_SIZE (1024)
#define AGS_VOLUME_UTIL_TEST_GET_DESTINATION_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

#define AGS_VOLUME_UTIL_TEST_SET_DESTINATION_BUFFER_SIZE (1024)
#define AGS_VOLUME_UTIL_TEST_SET_DESTINATION_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

#define AGS_VOLUME_UTIL_TEST_GET_SOURCE_BUFFER_SIZE (1024)
#define AGS_VOLUME_UTIL_TEST_GET_SOURCE_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

#define AGS_VOLUME_UTIL_TEST_SET_SOURCE_BUFFER_SIZE (1024)
#define AGS_VOLUME_UTIL_TEST_SET_SOURCE_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_volume_util_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_volume_util_test_clean_suite()
{
  return(0);
}

void
ags_volume_util_test_alloc()
{
  AgsVolumeUtil *volume_util;

  volume_util = ags_volume_util_alloc();

  CU_ASSERT(volume_util != NULL);

  CU_ASSERT(volume_util->destination == NULL);
  CU_ASSERT(volume_util->destination_stride == 1);

  CU_ASSERT(volume_util->source == NULL);
  CU_ASSERT(volume_util->source_stride == 1);
  
  CU_ASSERT(volume_util->buffer_length == 0);
  CU_ASSERT(volume_util->audio_buffer_util_format == AGS_VOLUME_UTIL_DEFAULT_AUDIO_BUFFER_UTIL_FORMAT);

  CU_ASSERT(volume_util->volume == 1.0);
}

void
ags_volume_util_test_copy()
{
  AgsVolumeUtil volume_util;
  AgsVolumeUtil *copy_volume_util;

  volume_util = (AgsVolumeUtil) {
    .destination = ags_stream_alloc(AGS_VOLUME_UTIL_TEST_COPY_BUFFER_SIZE,
				    AGS_VOLUME_UTIL_TEST_COPY_FORMAT),
    .source = ags_stream_alloc(AGS_VOLUME_UTIL_TEST_COPY_SOURCE_AUDIO_CHANNELS * AGS_VOLUME_UTIL_TEST_COPY_BUFFER_SIZE,
			       AGS_VOLUME_UTIL_TEST_COPY_FORMAT),
    .source_stride = AGS_VOLUME_UTIL_TEST_COPY_SOURCE_AUDIO_CHANNELS,
    .buffer_length = AGS_VOLUME_UTIL_TEST_COPY_BUFFER_SIZE,
    .volume = 0.5
  };

  copy_volume_util = ags_volume_util_copy(&volume_util);
  
  CU_ASSERT(copy_volume_util != NULL);
  
  CU_ASSERT(copy_volume_util->destination == volume_util.destination);
  CU_ASSERT(copy_volume_util->destination_stride == volume_util.destination_stride);
  
  CU_ASSERT(copy_volume_util->source == volume_util.source);
  CU_ASSERT(copy_volume_util->source_stride == volume_util.source_stride);
  
  CU_ASSERT(copy_volume_util->buffer_length == volume_util.buffer_length);
  CU_ASSERT(copy_volume_util->audio_buffer_util_format == volume_util.audio_buffer_util_format);

  CU_ASSERT(copy_volume_util->volume == volume_util.volume);
}

void
ags_volume_util_test_free()
{
  AgsVolumeUtil *volume_util;
  
  volume_util = (AgsVolumeUtil *) g_new(AgsVolumeUtil,
					1);

  CU_ASSERT(volume_util != NULL);

  volume_util->source = NULL;
  volume_util->destination = NULL;
  
  ags_volume_util_free(volume_util);
  
  volume_util = ags_volume_util_alloc();

  CU_ASSERT(volume_util != NULL);

  ags_volume_util_free(volume_util);
}

void
ags_volume_util_test_get_destination()
{
  AgsVolumeUtil volume_util;

  volume_util.destination = NULL;

  CU_ASSERT(ags_volume_util_get_destination(&volume_util) == NULL);
  
  volume_util.destination = ags_stream_alloc(AGS_VOLUME_UTIL_TEST_GET_DESTINATION_BUFFER_SIZE,
					     AGS_VOLUME_UTIL_TEST_GET_DESTINATION_FORMAT);

  CU_ASSERT(ags_volume_util_get_destination(&volume_util) == volume_util.destination);
}

void
ags_volume_util_test_set_destination()
{
  AgsVolumeUtil volume_util;

  gpointer destination;
  
  volume_util.destination = NULL;

  destination = ags_stream_alloc(AGS_VOLUME_UTIL_TEST_SET_DESTINATION_BUFFER_SIZE,
				 AGS_VOLUME_UTIL_TEST_SET_DESTINATION_FORMAT);

  ags_volume_util_set_destination(&volume_util,
				  destination);
  
  CU_ASSERT(volume_util.destination == destination);
}

void
ags_volume_util_test_get_destination_stride()
{
  AgsVolumeUtil volume_util;

  volume_util.destination_stride = 0;

  CU_ASSERT(ags_volume_util_get_destination_stride(&volume_util) == 0);
  
  volume_util.destination_stride = 1;

  CU_ASSERT(ags_volume_util_get_destination_stride(&volume_util) == volume_util.destination_stride);
}

void
ags_volume_util_test_set_destination_stride()
{
  AgsVolumeUtil volume_util;
  
  volume_util.destination_stride = 0;

  ags_volume_util_set_destination_stride(&volume_util,
					 1);
  
  CU_ASSERT(volume_util.destination_stride == 1);
}

void
ags_volume_util_test_get_source()
{
  AgsVolumeUtil volume_util;

  volume_util.source = NULL;

  CU_ASSERT(ags_volume_util_get_source(&volume_util) == NULL);
  
  volume_util.source = ags_stream_alloc(AGS_VOLUME_UTIL_TEST_GET_SOURCE_BUFFER_SIZE,
					AGS_VOLUME_UTIL_TEST_GET_SOURCE_FORMAT);

  CU_ASSERT(ags_volume_util_get_source(&volume_util) == volume_util.source);
}

void
ags_volume_util_test_set_source()
{
  AgsVolumeUtil volume_util;

  gpointer source;
  
  volume_util.source = NULL;

  source = ags_stream_alloc(AGS_VOLUME_UTIL_TEST_SET_SOURCE_BUFFER_SIZE,
			    AGS_VOLUME_UTIL_TEST_SET_SOURCE_FORMAT);
  
  ags_volume_util_set_source(&volume_util,
			     source);
  
  CU_ASSERT(volume_util.source == source);
}

void
ags_volume_util_test_get_source_stride()
{
  AgsVolumeUtil volume_util;

  volume_util.source_stride = 0;

  CU_ASSERT(ags_volume_util_get_source_stride(&volume_util) == 0);
  
  volume_util.source_stride = 1;

  CU_ASSERT(ags_volume_util_get_source_stride(&volume_util) == volume_util.source_stride);
}

void
ags_volume_util_test_set_source_stride()
{
  AgsVolumeUtil volume_util;
  
  volume_util.source_stride = 0;

  ags_volume_util_set_source_stride(&volume_util,
				    1);
  
  CU_ASSERT(volume_util.source_stride == 1);
}

void
ags_volume_util_test_get_buffer_length()
{
  AgsVolumeUtil volume_util;

  volume_util.buffer_length = 0;

  CU_ASSERT(ags_volume_util_get_buffer_length(&volume_util) == 0);
  
  volume_util.buffer_length = 512;

  CU_ASSERT(ags_volume_util_get_buffer_length(&volume_util) == volume_util.buffer_length);
}

void
ags_volume_util_test_set_buffer_length()
{
  AgsVolumeUtil volume_util;
  
  volume_util.buffer_length = 0;

  ags_volume_util_set_buffer_length(&volume_util,
				    512);
  
  CU_ASSERT(volume_util.buffer_length == 512);
}

void
ags_volume_util_test_get_audio_buffer_util_format()
{
  AgsVolumeUtil volume_util;

  volume_util.audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S16;

  CU_ASSERT(ags_volume_util_get_audio_buffer_util_format(&volume_util) == AGS_AUDIO_BUFFER_UTIL_S16);
  
  volume_util.audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_FLOAT;

  CU_ASSERT(ags_volume_util_get_audio_buffer_util_format(&volume_util) == volume_util.audio_buffer_util_format);
}

void
ags_volume_util_test_set_audio_buffer_util_format()
{
  AgsVolumeUtil volume_util;
  
  volume_util.audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S16;

  ags_volume_util_set_audio_buffer_util_format(&volume_util,
					       AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
  CU_ASSERT(volume_util.audio_buffer_util_format == AGS_AUDIO_BUFFER_UTIL_FLOAT);
}

void
ags_volume_util_test_get_volume()
{
  AgsVolumeUtil volume_util;

  volume_util.volume = 1.0;

  CU_ASSERT(ags_volume_util_get_volume(&volume_util) == 1.0);
  
  volume_util.volume = 0.25;

  CU_ASSERT(ags_volume_util_get_volume(&volume_util) == 0.25);
}

void
ags_volume_util_test_set_volume()
{
  AgsVolumeUtil volume_util;
  
  volume_util.volume = 1.0;

  ags_volume_util_set_volume(&volume_util,
			     0.35);
  
  CU_ASSERT(volume_util.volume == 0.35);
}

void
ags_volume_util_test_compute_s8()
{
  AgsVolumeUtil volume_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  volume_util = (AgsVolumeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S8,
    .volume = 0.5
  };
  
  ags_volume_util_compute_s8(&volume_util);
}

void
ags_volume_util_test_compute_s16()
{
  AgsVolumeUtil volume_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  volume_util = (AgsVolumeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S16,
    .volume = 0.5
  };
  
  ags_volume_util_compute_s16(&volume_util);
}

void
ags_volume_util_test_compute_s24()
{
  AgsVolumeUtil volume_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  volume_util = (AgsVolumeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S24,
    .volume = 0.5
  };
  
  ags_volume_util_compute_s24(&volume_util);
}

void
ags_volume_util_test_compute_s32()
{
  AgsVolumeUtil volume_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  volume_util = (AgsVolumeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S32,
    .volume = 0.5
  };
  
  ags_volume_util_compute_s32(&volume_util);
}

void
ags_volume_util_test_compute_s64()
{
  AgsVolumeUtil volume_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  volume_util = (AgsVolumeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S64,
    .volume = 0.5
  };
  
  ags_volume_util_compute_s64(&volume_util);
}

void
ags_volume_util_test_compute_float()
{
  AgsVolumeUtil volume_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_FLOAT);

  volume_util = (AgsVolumeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_FLOAT,
    .volume = 0.5
  };
  
  ags_volume_util_compute_float(&volume_util);
}

void
ags_volume_util_test_compute_double()
{
  AgsVolumeUtil volume_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_DOUBLE);

  volume_util = (AgsVolumeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_DOUBLE,
    .volume = 0.5
  };
  
  ags_volume_util_compute_double(&volume_util);
}

void
ags_volume_util_test_compute_complex()
{
  AgsVolumeUtil volume_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_COMPLEX);

  volume_util = (AgsVolumeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_COMPLEX,
    .volume = 0.5
  };
  
  ags_volume_util_compute_complex(&volume_util);
}

void
ags_volume_util_test_compute()
{
  AgsVolumeUtil volume_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  volume_util = (AgsVolumeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S16,
    .volume = 0.5
  };
  
  ags_volume_util_compute(&volume_util);
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
  pSuite = CU_add_suite("AgsVolumeUtilTest", ags_volume_util_test_init_suite, ags_volume_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsVolumeUtil alloc", ags_volume_util_test_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil copy", ags_volume_util_test_copy) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil free", ags_volume_util_test_free) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil get destination", ags_volume_util_test_get_destination) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil set destination", ags_volume_util_test_set_destination) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil get destination stride", ags_volume_util_test_get_destination_stride) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil set destination stride", ags_volume_util_test_set_destination_stride) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil get source", ags_volume_util_test_get_source) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil set source", ags_volume_util_test_set_source) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil get source stride", ags_volume_util_test_get_source_stride) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil set source stride", ags_volume_util_test_set_source_stride) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil get buffer length", ags_volume_util_test_get_buffer_length) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil set buffer length", ags_volume_util_test_set_buffer_length) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil get audio buffer util format", ags_volume_util_test_get_audio_buffer_util_format) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil set audio buffer util format", ags_volume_util_test_set_audio_buffer_util_format) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil get volume", ags_volume_util_test_get_volume) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil set volume", ags_volume_util_test_set_volume) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil compute s8", ags_volume_util_test_compute_s8) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil compute s16", ags_volume_util_test_compute_s16) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil compute s24", ags_volume_util_test_compute_s24) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil compute s32", ags_volume_util_test_compute_s32) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil compute s64", ags_volume_util_test_compute_s64) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil compute float", ags_volume_util_test_compute_float) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil compute double", ags_volume_util_test_compute_double) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil compute complex", ags_volume_util_test_compute_complex) == NULL) ||
     (CU_add_test(pSuite, "test of AgsVolumeUtil compute", ags_volume_util_test_compute) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
