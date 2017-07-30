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

int ags_acceleration_test_init_suite();
int ags_acceleration_test_clean_suite();

void ags_acceleration_test_duplicate();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_acceleration_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_acceleration_test_clean_suite()
{
  return(0);
}

void
ags_acceleration_test_duplicate()
{
  AgsAcceleration *acceleration, *copy;

  /* create acceleration */
  acceleration = g_object_new(AGS_TYPE_ACCELERATION,
			      "x", 0,
			      "y", 0,
			      "acceleration-name", "initial value",
			      NULL);

  /* assert duplicate */
  copy = ags_acceleration_duplicate(acceleration);

  CU_ASSERT(AGS_IS_ACCELERATION(copy) &&
	    copy->x == acceleration->x &&
	    copy->y == acceleration->y &&
	    !g_strcmp0(copy->acceleration_name,
		       acceleration->acceleration_name));

  /* create acceleration */
  acceleration = g_object_new(AGS_TYPE_ACCELERATION,
			      "x", 1024,
			      "y", 52,
			      "acceleration-name", "raise value",
			      NULL);

  /* assert duplicate */
  copy = ags_acceleration_duplicate(acceleration);

  CU_ASSERT(AGS_IS_ACCELERATION(copy) &&
	    copy->x == acceleration->x &&
	    copy->y == acceleration->y &&
	    !g_strcmp0(copy->acceleration_name,
		       acceleration->acceleration_name));
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
  pSuite = CU_add_suite("AgsAccelerationTest", ags_acceleration_test_init_suite, ags_acceleration_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsAcceleration duplicate", ags_acceleration_test_duplicate) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
