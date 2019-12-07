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

int ags_xml_authentication_test_init_suite();
int ags_xml_authentication_test_clean_suite();

void ags_xml_authentication_test_get_authentication_module();
void ags_xml_authentication_test_login();
void ags_xml_authentication_test_logout();
void ags_xml_authentication_test_generate_token();
void ags_xml_authentication_test_get_digest();
void ags_xml_authentication_test_is_session_active();

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_xml_authentication_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_xml_authentication_test_clean_suite()
{
  return(0);
}

void
ags_xml_authentication_test_get_authentication_module()
{
  //TODO:JK: implement me
}

void
ags_xml_authentication_test_login()
{
  //TODO:JK: implement me
}

void
ags_xml_authentication_test_logout()
{
  //TODO:JK: implement me
}

void
ags_xml_authentication_test_generate_token()
{
  //TODO:JK: implement me
}

void
ags_xml_authentication_test_get_digest()
{
  //TODO:JK: implement me
}

void
ags_xml_authentication_test_is_session_active()
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
  pSuite = CU_add_suite("AgsXmlAuthenticationTest", ags_xml_authentication_test_init_suite, ags_xml_authentication_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsXmlAuthentication get authentication module", ags_xml_authentication_test_get_authentication_module) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlAuthentication login", ags_xml_authentication_test_login) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlAuthentication logout", ags_xml_authentication_test_logout) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlAuthentication generate token", ags_xml_authentication_test_generate_token) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlAuthentication get digest", ags_xml_authentication_test_get_digest) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlAuthentication is session active", ags_xml_authentication_test_is_session_active) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
