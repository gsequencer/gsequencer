/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

int ags_input_test_init_suite();
int ags_input_test_clean_suite();

void ags_input_test_open_file();
void ags_input_test_apply_synth();
void ags_input_test_is_active();
void ags_input_test_next_active();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_input_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_input_test_clean_suite()
{
  return(0);
}

void
ags_input_test_open_file()
{
  //TODO:JK: implement me
}

void
ags_input_test_apply_synth()
{
  //TODO:JK: implement me
}

void
ags_input_test_is_active()
{
  //TODO:JK: implement me
}

void
ags_input_test_next_active()
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
  pSuite = CU_add_suite("AgsInputTest", ags_input_test_init_suite, ags_input_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  //  g_log_set_fatal_mask("GLib-GObject", // "Gtk" G_LOG_DOMAIN,
  //		       G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING);

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsInput open file", ags_input_test_open_file) == NULL) ||
     (CU_add_test(pSuite, "test of AgsInput apply synth", ags_input_test_apply_synth) == NULL) ||
     (CU_add_test(pSuite, "test of AgsInput is active", ags_input_test_is_active) == NULL) ||
     (CU_add_test(pSuite, "test of AgsInput next active", ags_input_test_next_active) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

