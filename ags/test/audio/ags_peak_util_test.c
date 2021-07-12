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
void ags_peak_util_test_get_audio_buffer_util_format();
void ags_peak_util_test_set_audio_buffer_util_format();
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
  //TODO:JK: implement me
}

void
ags_peak_util_test_copy()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_free()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_get_source()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_set_source()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_get_source_stride()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_set_source_stride()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_get_buffer_length()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_set_buffer_length()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_get_audio_buffer_util_format()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_set_audio_buffer_util_format()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_get_samplerate()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_set_samplerate()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_get_harmonic_rate()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_set_harmonic_rate()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_get_pressure_factor()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_set_pressure_factor()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_get_peak()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_set_peak()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_compute_s8()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_compute_s16()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_compute_s24()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_compute_s32()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_compute_s64()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_compute_float()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_compute_double()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_compute_complex()
{
  //TODO:JK: implement me
}

void
ags_peak_util_test_compute()
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
     (CU_add_test(pSuite, "test of AgsPeakUtil get audio buffer util format", ags_peak_util_test_get_audio_buffer_util_format) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPeakUtil set audio buffer util format", ags_peak_util_test_set_audio_buffer_util_format) == NULL) ||
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
