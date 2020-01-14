/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2019 Joël Krähemann
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

#include <ags/libags.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_server_test_init_suite();
int ags_server_test_clean_suite();

void ags_server_test_test_flags();
void ags_server_test_set_flags();
void ags_server_test_unset_flags();
void ags_server_test_info_alloc();
void ags_server_test_add_controller();
void ags_server_test_remove_controller();
void ags_server_test_start();
void ags_server_test_stop();
void ags_server_test_listen();
void ags_server_test_lookup();

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_server_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_server_test_clean_suite()
{
  return(0);
}

void
ags_server_test_test_flags()
{
  //TODO:JK: implement me
}

void
ags_server_test_set_flags()
{
  //TODO:JK: implement me
}

void
ags_server_test_unset_flags()
{
  //TODO:JK: implement me
}

void
ags_server_test_info_alloc()
{
  //TODO:JK: implement me
}

void
ags_server_test_add_controller()
{
  //TODO:JK: implement me
}

void
ags_server_test_remove_controller()
{
  //TODO:JK: implement me
}

void
ags_server_test_start()
{
  //TODO:JK: implement me
}

void
ags_server_test_stop()
{
  //TODO:JK: implement me
}

void
ags_server_test_listen()
{
  //TODO:JK: implement me
}

void
ags_server_test_lookup()
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
  pSuite = CU_add_suite("AgsServerTest\0", ags_server_test_init_suite, ags_server_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsServer test flags", ags_server_test_test_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsServer set flags", ags_server_test_set_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsServer unset flags", ags_server_test_unset_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsServer info alloc", ags_server_test_info_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsServer add controller", ags_server_test_add_controller) == NULL) ||
     (CU_add_test(pSuite, "test of AgsServer remove controller", ags_server_test_remove_controller) == NULL) ||
     (CU_add_test(pSuite, "test of AgsServer start", ags_server_test_start) == NULL) ||
     (CU_add_test(pSuite, "test of AgsServer stop", ags_server_test_stop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsServer listen", ags_server_test_listen) == NULL) ||
     (CU_add_test(pSuite, "test of AgsServer lookup", ags_server_test_lookup) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
