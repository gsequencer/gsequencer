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

int ags_envelope_util_test_init_suite();
int ags_envelope_util_test_clean_suite();

void ags_envelope_util_test_alloc();
void ags_envelope_util_test_copy();
void ags_envelope_util_test_free();
void ags_envelope_util_test_get_destination();
void ags_envelope_util_test_set_destination();
void ags_envelope_util_test_get_destination_stride();
void ags_envelope_util_test_set_destination_stride();
void ags_envelope_util_test_get_source();
void ags_envelope_util_test_set_source();
void ags_envelope_util_test_get_source_stride();
void ags_envelope_util_test_set_source_stride();
void ags_envelope_util_test_get_buffer_length();
void ags_envelope_util_test_set_buffer_length();
void ags_envelope_util_test_get_format();
void ags_envelope_util_test_set_format();
void ags_envelope_util_test_get_volume();
void ags_envelope_util_test_set_volume();
void ags_envelope_util_test_get_amount();
void ags_envelope_util_test_set_amount();
void ags_envelope_util_test_compute_s8();
void ags_envelope_util_test_compute_s16();
void ags_envelope_util_test_compute_s24();
void ags_envelope_util_test_compute_s32();
void ags_envelope_util_test_compute_s64();
void ags_envelope_util_test_compute_float();
void ags_envelope_util_test_compute_double();
void ags_envelope_util_test_compute_complex();
void ags_envelope_util_test_compute();

#define AGS_ENVELOPE_UTIL_TEST_COPY_BUFFER_SIZE (1024)
#define AGS_ENVELOPE_UTIL_TEST_COPY_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)
#define AGS_ENVELOPE_UTIL_TEST_COPY_SOURCE_AUDIO_CHANNELS (2)

#define AGS_ENVELOPE_UTIL_TEST_GET_DESTINATION_BUFFER_SIZE (1024)
#define AGS_ENVELOPE_UTIL_TEST_GET_DESTINATION_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

#define AGS_ENVELOPE_UTIL_TEST_SET_DESTINATION_BUFFER_SIZE (1024)
#define AGS_ENVELOPE_UTIL_TEST_SET_DESTINATION_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

#define AGS_ENVELOPE_UTIL_TEST_GET_SOURCE_BUFFER_SIZE (1024)
#define AGS_ENVELOPE_UTIL_TEST_GET_SOURCE_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

#define AGS_ENVELOPE_UTIL_TEST_SET_SOURCE_BUFFER_SIZE (1024)
#define AGS_ENVELOPE_UTIL_TEST_SET_SOURCE_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_envelope_util_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_envelope_util_test_clean_suite()
{
  return(0);
}

void
ags_envelope_util_test_alloc()
{
  AgsEnvelopeUtil *envelope_util;

  envelope_util = ags_envelope_util_alloc();

  CU_ASSERT(envelope_util != NULL);

  CU_ASSERT(envelope_util->destination == NULL);
  CU_ASSERT(envelope_util->destination_stride == 1);

  CU_ASSERT(envelope_util->source == NULL);
  CU_ASSERT(envelope_util->source_stride == 1);
  
  CU_ASSERT(envelope_util->buffer_length == 0);
  CU_ASSERT(envelope_util->format == AGS_ENVELOPE_UTIL_DEFAULT_FORMAT);

  CU_ASSERT(envelope_util->volume == 1.0);
  CU_ASSERT(envelope_util->amount == 0.0);
}

void
ags_envelope_util_test_copy()
{
  AgsEnvelopeUtil envelope_util;
  AgsEnvelopeUtil *copy_envelope_util;

  envelope_util = (AgsEnvelopeUtil) {
    .destination = ags_stream_alloc(AGS_ENVELOPE_UTIL_TEST_COPY_BUFFER_SIZE,
				    AGS_ENVELOPE_UTIL_TEST_COPY_FORMAT),
    .source = ags_stream_alloc(AGS_ENVELOPE_UTIL_TEST_COPY_SOURCE_AUDIO_CHANNELS * AGS_ENVELOPE_UTIL_TEST_COPY_BUFFER_SIZE,
			       AGS_ENVELOPE_UTIL_TEST_COPY_FORMAT),
    .source_stride = AGS_ENVELOPE_UTIL_TEST_COPY_SOURCE_AUDIO_CHANNELS,
    .buffer_length = AGS_ENVELOPE_UTIL_TEST_COPY_BUFFER_SIZE,
    .volume = 0.5,
    .amount = 0.25
  };

  copy_envelope_util = ags_envelope_util_copy(&envelope_util);
  
  CU_ASSERT(copy_envelope_util != NULL);
  
  CU_ASSERT(copy_envelope_util->destination == envelope_util.destination);
  CU_ASSERT(copy_envelope_util->destination_stride == envelope_util.destination_stride);
  
  CU_ASSERT(copy_envelope_util->source == envelope_util.source);
  CU_ASSERT(copy_envelope_util->source_stride == envelope_util.source_stride);
  
  CU_ASSERT(copy_envelope_util->buffer_length == envelope_util.buffer_length);
  CU_ASSERT(copy_envelope_util->format == envelope_util.format);

  CU_ASSERT(copy_envelope_util->volume == envelope_util.volume);
  CU_ASSERT(copy_envelope_util->amount == envelope_util.amount);
}

void
ags_envelope_util_test_free()
{
  AgsEnvelopeUtil *envelope_util;
  
  envelope_util = (AgsEnvelopeUtil *) g_new(AgsEnvelopeUtil,
					1);

  CU_ASSERT(envelope_util != NULL);

  envelope_util->source = NULL;
  envelope_util->destination = NULL;
  
  ags_envelope_util_free(envelope_util);
  
  envelope_util = ags_envelope_util_alloc();

  CU_ASSERT(envelope_util != NULL);

  ags_envelope_util_free(envelope_util);
}

void
ags_envelope_util_test_get_destination()
{
  AgsEnvelopeUtil envelope_util;

  envelope_util.destination = NULL;

  CU_ASSERT(ags_envelope_util_get_destination(&envelope_util) == NULL);
  
  envelope_util.destination = ags_stream_alloc(AGS_ENVELOPE_UTIL_TEST_GET_DESTINATION_BUFFER_SIZE,
					     AGS_ENVELOPE_UTIL_TEST_GET_DESTINATION_FORMAT);

  CU_ASSERT(ags_envelope_util_get_destination(&envelope_util) == envelope_util.destination);
}

void
ags_envelope_util_test_set_destination()
{
  AgsEnvelopeUtil envelope_util;

  gpointer destination;
  
  envelope_util.destination = NULL;

  destination = ags_stream_alloc(AGS_ENVELOPE_UTIL_TEST_SET_DESTINATION_BUFFER_SIZE,
				 AGS_ENVELOPE_UTIL_TEST_SET_DESTINATION_FORMAT);

  ags_envelope_util_set_destination(&envelope_util,
				  destination);
  
  CU_ASSERT(envelope_util.destination == destination);
}

void
ags_envelope_util_test_get_destination_stride()
{
  AgsEnvelopeUtil envelope_util;

  envelope_util.destination_stride = 0;

  CU_ASSERT(ags_envelope_util_get_destination_stride(&envelope_util) == 0);
  
  envelope_util.destination_stride = 1;

  CU_ASSERT(ags_envelope_util_get_destination_stride(&envelope_util) == envelope_util.destination_stride);
}

void
ags_envelope_util_test_set_destination_stride()
{
  AgsEnvelopeUtil envelope_util;
  
  envelope_util.destination_stride = 0;

  ags_envelope_util_set_destination_stride(&envelope_util,
					 1);
  
  CU_ASSERT(envelope_util.destination_stride == 1);
}

void
ags_envelope_util_test_get_source()
{
  AgsEnvelopeUtil envelope_util;

  envelope_util.source = NULL;

  CU_ASSERT(ags_envelope_util_get_source(&envelope_util) == NULL);
  
  envelope_util.source = ags_stream_alloc(AGS_ENVELOPE_UTIL_TEST_GET_SOURCE_BUFFER_SIZE,
					AGS_ENVELOPE_UTIL_TEST_GET_SOURCE_FORMAT);

  CU_ASSERT(ags_envelope_util_get_source(&envelope_util) == envelope_util.source);
}

void
ags_envelope_util_test_set_source()
{
  AgsEnvelopeUtil envelope_util;

  gpointer source;
  
  envelope_util.source = NULL;

  source = ags_stream_alloc(AGS_ENVELOPE_UTIL_TEST_SET_SOURCE_BUFFER_SIZE,
			    AGS_ENVELOPE_UTIL_TEST_SET_SOURCE_FORMAT);
  
  ags_envelope_util_set_source(&envelope_util,
			     source);
  
  CU_ASSERT(envelope_util.source == source);
}

void
ags_envelope_util_test_get_source_stride()
{
  AgsEnvelopeUtil envelope_util;

  envelope_util.source_stride = 0;

  CU_ASSERT(ags_envelope_util_get_source_stride(&envelope_util) == 0);
  
  envelope_util.source_stride = 1;

  CU_ASSERT(ags_envelope_util_get_source_stride(&envelope_util) == envelope_util.source_stride);
}

void
ags_envelope_util_test_set_source_stride()
{
  AgsEnvelopeUtil envelope_util;
  
  envelope_util.source_stride = 0;

  ags_envelope_util_set_source_stride(&envelope_util,
				    1);
  
  CU_ASSERT(envelope_util.source_stride == 1);
}

void
ags_envelope_util_test_get_buffer_length()
{
  AgsEnvelopeUtil envelope_util;

  envelope_util.buffer_length = 0;

  CU_ASSERT(ags_envelope_util_get_buffer_length(&envelope_util) == 0);
  
  envelope_util.buffer_length = 512;

  CU_ASSERT(ags_envelope_util_get_buffer_length(&envelope_util) == envelope_util.buffer_length);
}

void
ags_envelope_util_test_set_buffer_length()
{
  AgsEnvelopeUtil envelope_util;
  
  envelope_util.buffer_length = 0;

  ags_envelope_util_set_buffer_length(&envelope_util,
				    512);
  
  CU_ASSERT(envelope_util.buffer_length == 512);
}

void
ags_envelope_util_test_get_format()
{
  AgsEnvelopeUtil envelope_util;

  envelope_util.format = AGS_SOUNDCARD_SIGNED_16_BIT;

  CU_ASSERT(ags_envelope_util_get_format(&envelope_util) == AGS_SOUNDCARD_SIGNED_16_BIT);
  
  envelope_util.format = AGS_SOUNDCARD_FLOAT;

  CU_ASSERT(ags_envelope_util_get_format(&envelope_util) == envelope_util.format);
}

void
ags_envelope_util_test_set_format()
{
  AgsEnvelopeUtil envelope_util;
  
  envelope_util.format = AGS_SOUNDCARD_SIGNED_16_BIT;

  ags_envelope_util_set_format(&envelope_util,
					       AGS_SOUNDCARD_FLOAT);
  
  CU_ASSERT(envelope_util.format == AGS_SOUNDCARD_FLOAT);
}

void
ags_envelope_util_test_get_volume()
{
  AgsEnvelopeUtil envelope_util;

  envelope_util.volume = 1.0;

  CU_ASSERT(ags_envelope_util_get_volume(&envelope_util) == 1.0);
  
  envelope_util.volume = 0.25;

  CU_ASSERT(ags_envelope_util_get_volume(&envelope_util) == 0.25);
}

void
ags_envelope_util_test_set_volume()
{
  AgsEnvelopeUtil envelope_util;
  
  envelope_util.volume = 1.0;

  ags_envelope_util_set_volume(&envelope_util,
			       0.35);
  
  CU_ASSERT(envelope_util.volume == 0.35);
}

void
ags_envelope_util_test_get_amount()
{
  AgsEnvelopeUtil envelope_util;

  envelope_util.amount = 0.0;

  CU_ASSERT(ags_envelope_util_get_amount(&envelope_util) == 0.0);
  
  envelope_util.amount = 0.25;

  CU_ASSERT(ags_envelope_util_get_amount(&envelope_util) == 0.25);
}

void
ags_envelope_util_test_set_amount()
{
  AgsEnvelopeUtil envelope_util;
  
  envelope_util.amount = 0.0;

  ags_envelope_util_set_amount(&envelope_util,
			       0.25);
  
  CU_ASSERT(envelope_util.amount == 0.25);
}

void
ags_envelope_util_test_compute_s8()
{
  AgsEnvelopeUtil envelope_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  envelope_util = (AgsEnvelopeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_SIGNED_8_BIT,
    .volume = 0.5,
    .amount = 0.25
  };
  
  ags_envelope_util_compute_s8(&envelope_util);

  CU_ASSERT(envelope_util.volume > 0.5);
}

void
ags_envelope_util_test_compute_s16()
{
  AgsEnvelopeUtil envelope_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  envelope_util = (AgsEnvelopeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_SIGNED_16_BIT,
    .volume = 0.5,
    .amount = 0.25
  };
  
  ags_envelope_util_compute_s16(&envelope_util);

  CU_ASSERT(envelope_util.volume > 0.5);
}

void
ags_envelope_util_test_compute_s24()
{
  AgsEnvelopeUtil envelope_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  envelope_util = (AgsEnvelopeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_SIGNED_24_BIT,
    .volume = 0.5,
    .amount = 0.25
  };
  
  ags_envelope_util_compute_s24(&envelope_util);

  CU_ASSERT(envelope_util.volume > 0.5);
}

void
ags_envelope_util_test_compute_s32()
{
  AgsEnvelopeUtil envelope_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  envelope_util = (AgsEnvelopeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_SIGNED_32_BIT,
    .volume = 0.5,
    .amount = 0.25
  };
  
  ags_envelope_util_compute_s32(&envelope_util);

  CU_ASSERT(envelope_util.volume > 0.5);
}

void
ags_envelope_util_test_compute_s64()
{
  AgsEnvelopeUtil envelope_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  envelope_util = (AgsEnvelopeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_SIGNED_64_BIT,
    .volume = 0.5,
    .amount = 0.25
  };
  
  ags_envelope_util_compute_s64(&envelope_util);

  CU_ASSERT(envelope_util.volume > 0.5);
}

void
ags_envelope_util_test_compute_float()
{
  AgsEnvelopeUtil envelope_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_FLOAT);

  envelope_util = (AgsEnvelopeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_FLOAT,
    .volume = 0.5,
    .amount = 0.25
  };
  
  ags_envelope_util_compute_float(&envelope_util);

  CU_ASSERT(envelope_util.volume > 0.5);
}

void
ags_envelope_util_test_compute_double()
{
  AgsEnvelopeUtil envelope_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_DOUBLE);

  envelope_util = (AgsEnvelopeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_DOUBLE,
    .volume = 0.5,
    .amount = 0.25
  };
  
  ags_envelope_util_compute_double(&envelope_util);
}

void
ags_envelope_util_test_compute_complex()
{
  AgsEnvelopeUtil envelope_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_COMPLEX);

  envelope_util = (AgsEnvelopeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_COMPLEX,
    .volume = 0.5,
    .amount = 0.25
  };
  
  ags_envelope_util_compute_complex(&envelope_util);

  CU_ASSERT(envelope_util.volume > 0.5);
}

void
ags_envelope_util_test_compute()
{
  AgsEnvelopeUtil envelope_util;

  gpointer source;

  source = ags_stream_alloc(1024,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  envelope_util = (AgsEnvelopeUtil) {
    .destination = source,
    .source = source,
    .source_stride = 1,
    .buffer_length = 1024,
    .format = AGS_SOUNDCARD_SIGNED_16_BIT,
    .volume = 0.5,
    .amount = 0.25
  };
  
  ags_envelope_util_compute(&envelope_util);

  CU_ASSERT(envelope_util.volume > 0.5);
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
  pSuite = CU_add_suite("AgsEnvelopeUtilTest", ags_envelope_util_test_init_suite, ags_envelope_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsEnvelopeUtil alloc", ags_envelope_util_test_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil copy", ags_envelope_util_test_copy) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil free", ags_envelope_util_test_free) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil get destination", ags_envelope_util_test_get_destination) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil set destination", ags_envelope_util_test_set_destination) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil get destination stride", ags_envelope_util_test_get_destination_stride) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil set destination stride", ags_envelope_util_test_set_destination_stride) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil get source", ags_envelope_util_test_get_source) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil set source", ags_envelope_util_test_set_source) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil get source stride", ags_envelope_util_test_get_source_stride) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil set source stride", ags_envelope_util_test_set_source_stride) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil get buffer length", ags_envelope_util_test_get_buffer_length) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil set buffer length", ags_envelope_util_test_set_buffer_length) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil get format", ags_envelope_util_test_get_format) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil set format", ags_envelope_util_test_set_format) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil get volume", ags_envelope_util_test_get_volume) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil set volume", ags_envelope_util_test_set_volume) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil get amount", ags_envelope_util_test_get_amount) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil set amount", ags_envelope_util_test_set_amount) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil compute s8", ags_envelope_util_test_compute_s8) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil compute s16", ags_envelope_util_test_compute_s16) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil compute s24", ags_envelope_util_test_compute_s24) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil compute s32", ags_envelope_util_test_compute_s32) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil compute s64", ags_envelope_util_test_compute_s64) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil compute float", ags_envelope_util_test_compute_float) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil compute double", ags_envelope_util_test_compute_double) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil compute complex", ags_envelope_util_test_compute_complex) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil compute", ags_envelope_util_test_compute) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
