/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

int ags_synth_util_test_init_suite();
int ags_synth_util_test_clean_suite();

void ags_synth_util_test_alloc();
void ags_synth_util_test_copy();
void ags_synth_util_test_free();
void ags_synth_util_test_get_source();
void ags_synth_util_test_set_source();
void ags_synth_util_test_get_source_stride();
void ags_synth_util_test_set_source_stride();
void ags_synth_util_test_get_buffer_length();
void ags_synth_util_test_set_buffer_length();
void ags_synth_util_test_get_samplerate();
void ags_synth_util_test_set_samplerate();
void ags_synth_util_test_get_synth_oscillator_mode();
void ags_synth_util_test_set_synth_oscillator_mode();
void ags_synth_util_test_get_frequency();
void ags_synth_util_test_set_frequency();
void ags_synth_util_test_get_phase();
void ags_synth_util_test_set_phase();
void ags_synth_util_test_get_volume();
void ags_synth_util_test_set_volume();
void ags_synth_util_test_get_frame_count();
void ags_synth_util_test_set_frame_count();
void ags_synth_util_test_get_offset();
void ags_synth_util_test_set_offset();

void ags_synth_util_test_compute_sin_s8();
void ags_synth_util_test_compute_sin_s16();
void ags_synth_util_test_compute_sin_s24();
void ags_synth_util_test_compute_sin_s32();
void ags_synth_util_test_compute_sin_s64();
void ags_synth_util_test_compute_sin_float();
void ags_synth_util_test_compute_sin_double();
void ags_synth_util_test_compute_sin_complex();

void ags_synth_util_test_compute_sawtooth_s8();
void ags_synth_util_test_compute_sawtooth_s16();
void ags_synth_util_test_compute_sawtooth_s24();
void ags_synth_util_test_compute_sawtooth_s32();
void ags_synth_util_test_compute_sawtooth_s64();
void ags_synth_util_test_compute_sawtooth_float();
void ags_synth_util_test_compute_sawtooth_double();
void ags_synth_util_test_compute_sawtooth_complex();

void ags_synth_util_test_compute_triangle_s8();
void ags_synth_util_test_compute_triangle_s16();
void ags_synth_util_test_compute_triangle_s24();
void ags_synth_util_test_compute_triangle_s32();
void ags_synth_util_test_compute_triangle_s64();
void ags_synth_util_test_compute_triangle_float();
void ags_synth_util_test_compute_triangle_double();
void ags_synth_util_test_compute_triangle_complex();

void ags_synth_util_test_compute_square_s8();
void ags_synth_util_test_compute_square_s16();
void ags_synth_util_test_compute_square_s24();
void ags_synth_util_test_compute_square_s32();
void ags_synth_util_test_compute_square_s64();
void ags_synth_util_test_compute_square_float();
void ags_synth_util_test_compute_square_double();
void ags_synth_util_test_compute_square_complex();

void ags_synth_util_test_compute_impulse_s8();
void ags_synth_util_test_compute_impulse_s16();
void ags_synth_util_test_compute_impulse_s24();
void ags_synth_util_test_compute_impulse_s32();
void ags_synth_util_test_compute_impulse_s64();
void ags_synth_util_test_compute_impulse_float();
void ags_synth_util_test_compute_impulse_double();
void ags_synth_util_test_compute_impulse_complex();

#define AGS_SYNTH_UTIL_TEST_PHASE (0.0)
#define AGS_SYNTH_UTIL_TEST_VOLUME (1.0)
#define AGS_SYNTH_UTIL_TEST_SAMPLERATE (44100)
#define AGS_SYNTH_UTIL_TEST_OFFSET (0)
#define AGS_SYNTH_UTIL_TEST_FRAME_COUNT (1024)

gdouble test_freq[] = {
  13289.75,
  7040.00,
  880.0,
  440.0,
  220.0,
  27.5,
  8.18,
  -1.0,
};

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_synth_util_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_synth_util_test_clean_suite()
{
  return(0);
}

void
ags_synth_util_test_alloc()
{
  AgsSynthUtil *synth_util;

  synth_util = ags_synth_util_alloc();
  
  CU_ASSERT(synth_util != NULL);
}

void
ags_synth_util_test_copy()
{
  AgsSynthUtil *ptr;
  AgsSynthUtil synth_util;

  synth_util.source = ags_stream_alloc(2 * 1024,
				       AGS_SOUNDCARD_SIGNED_16_BIT);

  synth_util.source_stride = 2;
  
  synth_util.buffer_length = 2 * 1024;
  synth_util.format = AGS_SOUNDCARD_SIGNED_16_BIT;
  synth_util.samplerate = 44100;

  synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_IMPULSE;

  synth_util.frequency = 220.0;
  synth_util.phase = 0.0;
  synth_util.volume = 1.0;

  synth_util.frame_count = 4096;
  synth_util.offset = 0;

  ptr = ags_synth_util_copy(&synth_util);

  CU_ASSERT(ptr != NULL);
  
  CU_ASSERT(ptr->source == synth_util.source);
  CU_ASSERT(ptr->source_stride == synth_util.source_stride);
  
  CU_ASSERT(ptr->buffer_length == synth_util.buffer_length);
  CU_ASSERT(ptr->format == synth_util.format);
  CU_ASSERT(ptr->samplerate == synth_util.samplerate);

  CU_ASSERT(ptr->frequency == synth_util.frequency);
  CU_ASSERT(ptr->phase == synth_util.phase);
  CU_ASSERT(ptr->volume == synth_util.volume);

  CU_ASSERT(ptr->frame_count == synth_util.frame_count);
  CU_ASSERT(ptr->offset == synth_util.offset);
}

void
ags_synth_util_test_free()
{
  AgsSynthUtil *synth_util;

  synth_util = ags_synth_util_alloc();
  
  CU_ASSERT(synth_util != NULL);

  ags_synth_util_free(synth_util);
}

void
ags_synth_util_test_get_source()
{
  AgsSynthUtil synth_util;

  synth_util.source = ags_stream_alloc(2 * 1024,
				       AGS_SOUNDCARD_SIGNED_16_BIT);

  CU_ASSERT(ags_synth_util_get_source(&synth_util) == synth_util.source);
}

void
ags_synth_util_test_set_source()
{
  AgsSynthUtil synth_util;

  gpointer source;
  
  source = ags_stream_alloc(2 * 1024,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  ags_synth_util_set_source(&synth_util,
			    source);
  
  CU_ASSERT(synth_util.source == synth_util.source);
}

void
ags_synth_util_test_get_source_stride()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_set_source_stride()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_get_buffer_length()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_set_buffer_length()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_get_samplerate()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_set_samplerate()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_get_synth_oscillator_mode()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_set_synth_oscillator_mode()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_get_frequency()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_set_frequency()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_get_phase()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_set_phase()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_get_volume()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_set_volume()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_get_frame_count()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_set_frame_count()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_get_offset()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_set_offset()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_sin_s8()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_sin_s16()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_sin_s24()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_sin_s32()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_sin_s64()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_sin_float()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_sin_double()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_sin_complex()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_sawtooth_s8()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_sawtooth_s16()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_sawtooth_s24()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_sawtooth_s32()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_sawtooth_s64()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_sawtooth_float()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_sawtooth_double()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_sawtooth_complex()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_triangle_s8()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_triangle_s16()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_triangle_s24()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_triangle_s32()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_triangle_s64()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_triangle_float()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_triangle_double()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_triangle_complex()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_square_s8()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_square_s16()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_square_s24()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_square_s32()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_square_s64()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_square_float()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_square_double()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_square_complex()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_impulse_s8()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_impulse_s16()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_impulse_s24()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_impulse_s32()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_impulse_s64()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_impulse_float()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_impulse_double()
{
  //TODO:JK: implement me
}

void
ags_synth_util_test_compute_impulse_complex()
{
  //TODO:JK: implement me
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
  pSuite = CU_add_suite("AgsSynthUtilTest", ags_synth_util_test_init_suite, ags_synth_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_synth_util.c alloc", ags_synth_util_test_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c copy", ags_synth_util_test_copy) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c free", ags_synth_util_test_free) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c get source", ags_synth_util_test_get_source) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c set source", ags_synth_util_test_set_source) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c get source stride", ags_synth_util_test_get_source_stride) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c set source stride", ags_synth_util_test_set_source_stride) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c get buffer length", ags_synth_util_test_get_buffer_length) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c set buffer length", ags_synth_util_test_set_buffer_length) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c get samplerate", ags_synth_util_test_get_samplerate) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c set samplerate", ags_synth_util_test_set_samplerate) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c get synth oscillator mode", ags_synth_util_test_get_synth_oscillator_mode) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c set synth oscillator mode", ags_synth_util_test_set_synth_oscillator_mode) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c get frequency", ags_synth_util_test_get_frequency) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c set frequency", ags_synth_util_test_set_frequency) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c get phase", ags_synth_util_test_get_phase) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c set phase", ags_synth_util_test_set_phase) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c get volume", ags_synth_util_test_get_volume) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c set volume", ags_synth_util_test_set_volume) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c get frame count", ags_synth_util_test_get_frame_count) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c set frame count", ags_synth_util_test_set_frame_count) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c get offset", ags_synth_util_test_get_offset) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c set offset", ags_synth_util_test_set_offset) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute sin s8", ags_synth_util_test_compute_sin_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute sin s16", ags_synth_util_test_compute_sin_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute sin s24", ags_synth_util_test_compute_sin_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute sin s32", ags_synth_util_test_compute_sin_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute sin s64", ags_synth_util_test_compute_sin_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute sin float", ags_synth_util_test_compute_sin_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute sin double", ags_synth_util_test_compute_sin_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute sin complex", ags_synth_util_test_compute_sin_complex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute sawtooth s8", ags_synth_util_test_compute_sawtooth_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute sawtooth s16", ags_synth_util_test_compute_sawtooth_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute sawtooth s24", ags_synth_util_test_compute_sawtooth_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute sawtooth s32", ags_synth_util_test_compute_sawtooth_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute sawtooth s64", ags_synth_util_test_compute_sawtooth_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute sawtooth float", ags_synth_util_test_compute_sawtooth_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute sawtooth double", ags_synth_util_test_compute_sawtooth_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute sawtooth complex", ags_synth_util_test_compute_sawtooth_complex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute triangle s8", ags_synth_util_test_compute_triangle_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute triangle s16", ags_synth_util_test_compute_triangle_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute triangle s24", ags_synth_util_test_compute_triangle_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute triangle s32", ags_synth_util_test_compute_triangle_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute triangle s64", ags_synth_util_test_compute_triangle_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute triangle float", ags_synth_util_test_compute_triangle_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute triangle double", ags_synth_util_test_compute_triangle_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute triangle complex", ags_synth_util_test_compute_triangle_complex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute square s8", ags_synth_util_test_compute_square_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute square s16", ags_synth_util_test_compute_square_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute square s24", ags_synth_util_test_compute_square_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute square s32", ags_synth_util_test_compute_square_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute square s64", ags_synth_util_test_compute_square_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute square float", ags_synth_util_test_compute_square_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute square double", ags_synth_util_test_compute_square_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute square complex", ags_synth_util_test_compute_square_complex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute impulse s16", ags_synth_util_test_compute_impulse_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute impulse s24", ags_synth_util_test_compute_impulse_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute impulse s32", ags_synth_util_test_compute_impulse_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute impulse s64", ags_synth_util_test_compute_impulse_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute impulse float", ags_synth_util_test_compute_impulse_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute impulse double", ags_synth_util_test_compute_impulse_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_synth_util.c compute impulse complex", ags_synth_util_test_compute_impulse_complex) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
