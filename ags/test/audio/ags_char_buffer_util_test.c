/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2018-2020 Joël Krähemann
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

int ags_char_buffer_util_test_init_suite();
int ags_char_buffer_util_test_clean_suite();

void ags_char_buffer_util_test_copy_s8_to_cbuffer();
void ags_char_buffer_util_test_copy_s16_to_cbuffer();
void ags_char_buffer_util_test_copy_s24_to_cbuffer();
void ags_char_buffer_util_test_copy_s32_to_cbuffer();
void ags_char_buffer_util_test_copy_s64_to_cbuffer();
void ags_char_buffer_util_test_copy_float_to_cbuffer();
void ags_char_buffer_util_test_copy_double_to_cbuffer();
void ags_char_buffer_util_test_copy_cbuffer_to_s8();
void ags_char_buffer_util_test_copy_cbuffer_to_s16();
void ags_char_buffer_util_test_copy_cbuffer_to_s24();
void ags_char_buffer_util_test_copy_cbuffer_to_s32();
void ags_char_buffer_util_test_copy_cbuffer_to_s64();
void ags_char_buffer_util_test_copy_cbuffer_to_float();
void ags_char_buffer_util_test_copy_cbuffer_to_double();
void ags_char_buffer_util_test_copy_buffer_to_buffer();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_char_buffer_util_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_char_buffer_util_test_clean_suite()
{
  return(0);
}

void
ags_char_buffer_util_test_copy_s8_to_cbuffer()
{
  CU_ASSERT(TRUE);
  
  //TODO:JK: implement me  
}

void
ags_char_buffer_util_test_copy_s16_to_cbuffer()
{
  //TODO:JK: implement me  
}

void
ags_char_buffer_util_test_copy_s24_to_cbuffer()
{
  //TODO:JK: implement me  
}

void
ags_char_buffer_util_test_copy_s32_to_cbuffer()
{
  //TODO:JK: implement me  
}

void
ags_char_buffer_util_test_copy_s64_to_cbuffer()
{
  //TODO:JK: implement me  
}

void
ags_char_buffer_util_test_copy_float_to_cbuffer()
{
  //TODO:JK: implement me  
}

void
ags_char_buffer_util_test_copy_double_to_cbuffer()
{
  //TODO:JK: implement me  
}

void
ags_char_buffer_util_test_copy_cbuffer_to_s8()
{
  //TODO:JK: implement me  
}

void
ags_char_buffer_util_test_copy_cbuffer_to_s16()
{
  //TODO:JK: implement me  
}

void
ags_char_buffer_util_test_copy_cbuffer_to_s24()
{
  //TODO:JK: implement me  
}

void
ags_char_buffer_util_test_copy_cbuffer_to_s32()
{
  //TODO:JK: implement me  
}

void
ags_char_buffer_util_test_copy_cbuffer_to_s64()
{
  //TODO:JK: implement me  
}

void
ags_char_buffer_util_test_copy_cbuffer_to_float()
{
  //TODO:JK: implement me  
}

void
ags_char_buffer_util_test_copy_cbuffer_to_double()
{
  //TODO:JK: implement me  
}

void
ags_char_buffer_util_test_copy_buffer_to_buffer()
{
  //TODO:JK: implement me  
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
  pSuite = CU_add_suite("AgsCharBufferUtilTest", ags_char_buffer_util_test_init_suite, ags_char_buffer_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_char_buffer_util.c copy s8 to cbuffer", ags_char_buffer_util_test_copy_s8_to_cbuffer) == NULL) ||
     (CU_add_test(pSuite, "test of ags_char_buffer_util.c copy s16 to cbuffer", ags_char_buffer_util_test_copy_s16_to_cbuffer) == NULL) ||
     (CU_add_test(pSuite, "test of ags_char_buffer_util.c copy s24 to cbuffer", ags_char_buffer_util_test_copy_s24_to_cbuffer) == NULL) ||
     (CU_add_test(pSuite, "test of ags_char_buffer_util.c copy s32 to cbuffer", ags_char_buffer_util_test_copy_s32_to_cbuffer) == NULL) ||
     (CU_add_test(pSuite, "test of ags_char_buffer_util.c copy s64 to cbuffer", ags_char_buffer_util_test_copy_s64_to_cbuffer) == NULL) ||
     (CU_add_test(pSuite, "test of ags_char_buffer_util.c copy float to cbuffer", ags_char_buffer_util_test_copy_float_to_cbuffer) == NULL) ||
     (CU_add_test(pSuite, "test of ags_char_buffer_util.c copy double to cbuffer", ags_char_buffer_util_test_copy_double_to_cbuffer) == NULL) ||
     (CU_add_test(pSuite, "test of ags_char_buffer_util.c copy cbuffer to s8", ags_char_buffer_util_test_copy_cbuffer_to_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_char_buffer_util.c copy cbuffer to s16", ags_char_buffer_util_test_copy_cbuffer_to_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_char_buffer_util.c copy cbuffer to s24", ags_char_buffer_util_test_copy_cbuffer_to_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_char_buffer_util.c copy cbuffer to s32", ags_char_buffer_util_test_copy_cbuffer_to_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_char_buffer_util.c copy cbuffer to s64", ags_char_buffer_util_test_copy_cbuffer_to_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_char_buffer_util.c copy cbuffer to float", ags_char_buffer_util_test_copy_cbuffer_to_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_char_buffer_util.c copy cbuffer to double", ags_char_buffer_util_test_copy_cbuffer_to_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_char_buffer_util.c copy buffer to buffer", ags_char_buffer_util_test_copy_buffer_to_buffer) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

