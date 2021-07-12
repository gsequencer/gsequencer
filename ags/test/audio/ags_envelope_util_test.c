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
void ags_envelope_util_test_get_audio_buffer_util_format();
void ags_envelope_util_test_set_audio_buffer_util_format();
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
  //TODO:JK: implement me
}

void
ags_envelope_util_test_copy()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_free()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_get_destination()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_set_destination()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_get_destination_stride()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_set_destination_stride()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_get_source()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_set_source()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_get_source_stride()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_set_source_stride()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_get_buffer_length()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_set_buffer_length()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_get_audio_buffer_util_format()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_set_audio_buffer_util_format()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_get_volume()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_set_volume()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_get_amount()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_set_amount()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_compute_s8()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_compute_s16()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_compute_s24()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_compute_s32()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_compute_s64()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_compute_float()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_compute_double()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_compute_complex()
{
  //TODO:JK: implement me
}

void
ags_envelope_util_test_compute()
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
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil get audio buffer util format", ags_envelope_util_test_get_audio_buffer_util_format) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEnvelopeUtil set audio buffer util format", ags_envelope_util_test_set_audio_buffer_util_format) == NULL) ||
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
