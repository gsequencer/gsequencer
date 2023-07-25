/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2023 Joël Krähemann
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

#include <stdlib.h>

int ags_midi_ci_1_1_util_test_init_suite();
int ags_midi_ci_1_1_util_test_clean_suite();

void ags_midi_ci_1_1_util_test_alloc();
void ags_midi_ci_1_1_util_test_free();
void ags_midi_ci_1_1_util_test_copy();
void ags_midi_ci_1_1_util_test_generate_muid();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_midi_ci_1_1_util_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_midi_ci_1_1_util_test_clean_suite()
{
  return(0);
}

void
ags_midi_ci_1_1_util_test_alloc()
{
  AgsMidiCI_1_1_Util *midi_ci_1_1_util;

  midi_ci_1_1_util = ags_midi_ci_1_1_util_alloc();

  CU_ASSERT(midi_ci_1_1_util != NULL);
}

void
ags_midi_ci_1_1_util_test_free()
{
  AgsMidiCI_1_1_Util *midi_ci_1_1_util;

  midi_ci_1_1_util = ags_midi_ci_1_1_util_alloc();

  ags_midi_ci_1_1_util_free(midi_ci_1_1_util);
}

void
ags_midi_ci_1_1_util_test_copy()
{
  AgsMidiCI_1_1_Util *a, *b;

  a = ags_midi_ci_1_1_util_alloc();

  b = ags_midi_ci_1_1_util_copy(a);
  
  CU_ASSERT(b != NULL);
}

void
ags_midi_ci_1_1_util_test_generate_muid()
{
  AgsMidiCI_1_1_Util *midi_ci_1_1_util;

  AgsMUID muid;
  
  midi_ci_1_1_util = ags_midi_ci_1_1_util_alloc();

  muid = ags_midi_ci_1_1_util_generate_muid(midi_ci_1_1_util);
  
  CU_ASSERT(muid != AGS_MIDI_CI_1_1_UTIL_BROADCAST_MUID);
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
  pSuite = CU_add_suite("AgsMidiCI_1_1UtilTest", ags_midi_ci_1_1_util_test_init_suite, ags_midi_ci_1_1_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_midi_ci_1_1_util.c alloc", ags_midi_ci_1_1_util_test_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_1_1_util.c free", ags_midi_ci_1_1_util_test_free) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_1_1_util.c copy", ags_midi_ci_1_1_util_test_copy) == NULL) ||
     (CU_add_test(pSuite, "test of ags_midi_ci_1_1_util.c generate MUID", ags_midi_ci_1_1_util_test_generate_muid) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}


