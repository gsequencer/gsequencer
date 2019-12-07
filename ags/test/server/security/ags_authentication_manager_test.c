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

int ags_authentication_manager_test_init_suite();
int ags_authentication_manager_test_clean_suite();

void ags_authentication_manager_test_login_info_alloc();
void ags_authentication_manager_test_login_info_free();
void ags_authentication_manager_test_login_info_ref();
void ags_authentication_manager_test_login_info_unref();
void ags_authentication_manager_test_get_authentication();
void ags_authentication_manager_test_add_authentication();
void ags_authentication_manager_test_remove_authentication();
void ags_authentication_manager_test_get_session_timeout();
void ags_authentication_manager_test_lookup_login();
void ags_authentication_manager_test_insert_login();
void ags_authentication_manager_test_remove_login();
void ags_authentication_manager_test_login();
void ags_authentication_manager_test_logout();
void ags_authentication_manager_test_get_digest();
void ags_authentication_manager_test_is_session_active();

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_authentication_manager_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_authentication_manager_test_clean_suite()
{
  return(0);
}

void
ags_authentication_manager_test_login_info_alloc()
{
  //TODO:JK: implement me
}

void
ags_authentication_manager_test_login_info_free()
{
  //TODO:JK: implement me
}

void
ags_authentication_manager_test_login_info_ref()
{
  //TODO:JK: implement me
}

void
ags_authentication_manager_test_login_info_unref()
{
  //TODO:JK: implement me
}

void
ags_authentication_manager_test_get_authentication()
{
  //TODO:JK: implement me
}

void
ags_authentication_manager_test_add_authentication()
{
  //TODO:JK: implement me
}

void
ags_authentication_manager_test_remove_authentication()
{
  //TODO:JK: implement me
}

void
ags_authentication_manager_test_get_session_timeout()
{
  //TODO:JK: implement me
}

void
ags_authentication_manager_test_lookup_login()
{
  //TODO:JK: implement me
}

void
ags_authentication_manager_test_insert_login()
{
  //TODO:JK: implement me
}

void
ags_authentication_manager_test_remove_login()
{
  //TODO:JK: implement me
}

void
ags_authentication_manager_test_login()
{
  //TODO:JK: implement me
}

void
ags_authentication_manager_test_logout()
{
  //TODO:JK: implement me
}

void
ags_authentication_manager_test_get_digest()
{
  //TODO:JK: implement me
}

void
ags_authentication_manager_test_is_session_active()
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
  pSuite = CU_add_suite("AgsAuthenticationManagerTest", ags_authentication_manager_test_init_suite, ags_authentication_manager_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsAuthenticationManager login info alloc", ags_authentication_manager_test_login_info_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAuthenticationManager login info free", ags_authentication_manager_test_login_info_free) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAuthenticationManager login info ref", ags_authentication_manager_test_login_info_ref) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAuthenticationManager login info unref", ags_authentication_manager_test_login_info_unref) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAuthenticationManager get authentication", ags_authentication_manager_test_get_authentication) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAuthenticationManager add authentication", ags_authentication_manager_test_add_authentication) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAuthenticationManager remove authentication", ags_authentication_manager_test_remove_authentication) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAuthenticationManager get session timeout", ags_authentication_manager_test_get_session_timeout) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAuthenticationManager lookup login", ags_authentication_manager_test_lookup_login) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAuthenticationManager insert login", ags_authentication_manager_test_insert_login) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAuthenticationManager remove login", ags_authentication_manager_test_remove_login) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAuthenticationManager login", ags_authentication_manager_test_login) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAuthenticationManager logout", ags_authentication_manager_test_logout) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAuthenticationManager get digest", ags_authentication_manager_test_get_digest) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAuthenticationManager is session active", ags_authentication_manager_test_is_session_active) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
