/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2017 Joël Krähemann
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

#include <ags/lib/ags_complex.h>

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <math.h>

int ags_complex_test_init_suite();
int ags_complex_test_clean_suite();

void ags_complex_test_copy();
void ags_complex_test_get();
void ags_complex_test_set();

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_complex_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_complex_test_clean_suite()
{
  return(0);
}

void
ags_complex_test_copy()
{
  AgsComplex a;
  AgsComplex *ptr;

  /* test x = 1.0 */
  a[0] = 1.0;
  a[1] = 0.0;

  ptr = ags_complex_copy(&a);

  CU_ASSERT(ptr[0][0] == a[0] &&
	    ptr[0][1] == a[1]);

  /* test y = 1.0 */
  a[0] = 0.0;
  a[1] = 1.0;

  ptr = ags_complex_copy(&a);

  CU_ASSERT(ptr[0][0] == a[0] &&
	    ptr[0][1] == a[1]);

  /* test x = 1.25, y = 2 * M_PI */
  a[0] = 1.25;
  a[1] = 2.0 * M_PI;

  ptr = ags_complex_copy(&a);

  CU_ASSERT(ptr[0][0] == a[0] &&
	    ptr[0][1] == a[1]);

  /* test x = 2 * M_PI, y = 1.25 */
  a[0] = 2.0 * M_PI;
  a[1] = 1.25;

  ptr = ags_complex_copy(&a);

  CU_ASSERT(ptr[0][0] == a[0] &&
	    ptr[0][1] == a[1]);
}

void
ags_complex_test_get()
{
  AgsComplex a;

  complex z;
  
  /* test x = 1.0 */
  a[0] = 1.0;
  a[1] = 0.0;

  z = ags_complex_get(&a);

  CU_ASSERT(z == (a[0] + a[1] * I));
  
  /* test y = 1.0 */
  a[0] = 0.0;
  a[1] = 1.0;

  z = ags_complex_get(&a);

  CU_ASSERT(z == (a[0] + a[1] * I));

  /* test x = 1.25, y = 2 * M_PI */
  a[0] = 1.25;
  a[1] = 2.0 * M_PI;

  z = ags_complex_get(&a);

  CU_ASSERT(z == (a[0] + a[1] * I));

  /* test x = 2 * M_PI, y = 1.25 */
  a[0] = 2.0 * M_PI;
  a[1] = 1.25;

  z = ags_complex_get(&a);

  CU_ASSERT(z == (a[0] + a[1] * I));
}

void
ags_complex_test_set()
{
  AgsComplex a;

  complex z;

  /* test x = 1.0 */
  z = 1.0 + 0.0 * I;
  
  ags_complex_set(&a, z);

  CU_ASSERT(a[0] == 1.0 &&
	    a[1] == 0.0);
  
  /* test y = 1.0 */
  z = 0.0 + 1.0 * I;

  ags_complex_set(&a, z);

  CU_ASSERT(a[0] == 0.0 &&
	    a[1] == 1.0);
  
  /* test x = 1.25, y = 2 * M_PI */
  z = 1.25 + (2.0 * M_PI) * I;

  ags_complex_set(&a, z);

  CU_ASSERT(a[0] == 1.25 &&
	    a[1] == 2.0 * M_PI);
  
  /* test x = 2 * M_PI, y = 1.25 */
  z = (2.0 * M_PI) + 1.25 * I;

  ags_complex_set(&a, z);
  
  CU_ASSERT(a[0] == 2.0 * M_PI &&
	    a[1] == 1.25);
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
  pSuite = CU_add_suite("AgsComplexTest\0", ags_complex_test_init_suite, ags_complex_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsComplex copy\0", ags_complex_test_copy) == NULL) ||
     (CU_add_test(pSuite, "test of AgsComplex get\0", ags_complex_test_get) == NULL) ||
     (CU_add_test(pSuite, "test of AgsComplex set\0", ags_complex_test_set) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
