/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/libags.h>

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <math.h>

int ags_endian_test_init_suite();
int ags_endian_test_clean_suite();

void ags_endian_test_host_is_le();
void ags_endian_test_host_is_be();
void ags_endian_test_swap_float();

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_endian_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_endian_test_clean_suite()
{
  return(0);
}

void
ags_endian_test_host_is_le()
{
  gboolean host_is_le;

  host_is_le = ags_endian_host_is_le();
}

void
ags_endian_test_host_is_be()
{
  gboolean host_is_be;

  host_is_be = ags_endian_host_is_be();
}

void
ags_endian_test_swap_float()
{
  gfloat val;
  gfloat swapped_val;

  val = 0.0;

  swapped_val = ags_endian_swap_float(val);
  
  val = 1.0;

  swapped_val = ags_endian_swap_float(val);

  val = -1.0;

  swapped_val = ags_endian_swap_float(val);

  val = 2.0 * M_PI;
  
  swapped_val = ags_endian_swap_float(val);
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
  pSuite = CU_add_suite("AgsEndianTest", ags_endian_test_init_suite, ags_endian_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsEndian host is le", ags_endian_test_host_is_le) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEndian host is be", ags_endian_test_host_is_be) == NULL) ||
     (CU_add_test(pSuite, "test of AgsEndian swap float", ags_endian_test_swap_float) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
