/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

int ags_buffer_test_init_suite();
int ags_buffer_test_clean_suite();

void ags_buffer_test_duplicate();

#define AGS_BUFFER_TEST_DUPLICATE_SAMPLERATE_0 (44100)
#define AGS_BUFFER_TEST_DUPLICATE_BUFFER_SIZE_0 (1024)
#define AGS_BUFFER_TEST_DUPLICATE_FORMAT_0 (AGS_SOUNDCARD_SIGNED_16_BIT)
#define AGS_BUFFER_TEST_DUPLICATE_X_0 (17 * AGS_BUFFER_TEST_DUPLICATE_BUFFER_SIZE_0)
#define AGS_BUFFER_TEST_DUPLICATE_SAMPLERATE_1 (44100)
#define AGS_BUFFER_TEST_DUPLICATE_BUFFER_SIZE_1 (1024)
#define AGS_BUFFER_TEST_DUPLICATE_FORMAT_1 (AGS_SOUNDCARD_SIGNED_16_BIT)
#define AGS_BUFFER_TEST_DUPLICATE_X_1 (17 * AGS_BUFFER_TEST_DUPLICATE_BUFFER_SIZE_1)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_buffer_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_buffer_test_clean_suite()
{
  return(0);
}

void
ags_buffer_test_duplicate()
{
  AgsBuffer *buffer, *copy_buffer;

  /* create buffer - #0 */
  buffer = g_object_new(AGS_TYPE_BUFFER,
			"x", AGS_BUFFER_TEST_DUPLICATE_X_0,
			"samplerate", AGS_BUFFER_TEST_DUPLICATE_SAMPLERATE_0,
			"buffer-size", AGS_BUFFER_TEST_DUPLICATE_BUFFER_SIZE_0,
			"format", AGS_BUFFER_TEST_DUPLICATE_FORMAT_0,
			NULL);

  /* assert duplicate - #0 */
  copy_buffer = ags_buffer_duplicate(buffer);

  CU_ASSERT(AGS_IS_BUFFER(copy_buffer) &&
	    copy_buffer->x == buffer->x &&
	    copy_buffer->samplerate == buffer->samplerate &&
	    copy_buffer->buffer_size == buffer->buffer_size &&
	    copy_buffer->format == buffer->format);

  /* create buffer - #1 */
  buffer = g_object_new(AGS_TYPE_BUFFER,
			"x", AGS_BUFFER_TEST_DUPLICATE_X_1,
			"samplerate", AGS_BUFFER_TEST_DUPLICATE_SAMPLERATE_1,
			"buffer-size", AGS_BUFFER_TEST_DUPLICATE_BUFFER_SIZE_1,
			"format", AGS_BUFFER_TEST_DUPLICATE_FORMAT_1,
			NULL);

  /* assert duplicate - #1 */
  copy_buffer = ags_buffer_duplicate(buffer);

  CU_ASSERT(AGS_IS_BUFFER(copy_buffer) &&
	    copy_buffer->x == buffer->x &&
	    copy_buffer->samplerate == buffer->samplerate &&
	    copy_buffer->buffer_size == buffer->buffer_size &&
	    copy_buffer->format == buffer->format);
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
  pSuite = CU_add_suite("AgsBufferTest", ags_buffer_test_init_suite, ags_buffer_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsBuffer duplicate", ags_buffer_test_duplicate) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
