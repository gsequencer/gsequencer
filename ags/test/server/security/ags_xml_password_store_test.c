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

int ags_xml_password_store_test_init_suite();
int ags_xml_password_store_test_clean_suite();

void ags_xml_password_store_test_set_login_name();
void ags_xml_password_store_test_get_login_name();
void ags_xml_password_store_test_set_password();
void ags_xml_password_store_test_get_password();
void ags_xml_password_store_test_encrypt_password();
void ags_xml_password_store_test_open();
void ags_xml_password_store_test_find_login();

AgsServerApplicationContext *server_application_context;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_xml_password_store_test_init_suite()
{
  AgsConfig *config;

  config = ags_config_get_instance();

  server_application_context = (AgsApplicationContext *) ags_server_application_context_new();
  g_object_ref(server_application_context);

  ags_application_context_prepare(server_application_context);
  ags_application_context_setup(server_application_context);

  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_xml_password_store_test_clean_suite()
{
  return(0);
}

void
ags_xml_password_store_test_set_login_name()
{
  AgsXmlPasswordStore *xml_password_store;

  xml_password_store = ags_xml_password_store_new();
  
  //TODO:JK: implement me
}

void
ags_xml_password_store_test_get_login_name()
{
  //TODO:JK: implement me
}

void
ags_xml_password_store_test_set_password()
{
  //TODO:JK: implement me
}

void
ags_xml_password_store_test_get_password()
{
  //TODO:JK: implement me
}

void
ags_xml_password_store_test_encrypt_password()
{
  //TODO:JK: implement me
}

void
ags_xml_password_store_test_open()
{
  //TODO:JK: implement me
}

void
ags_xml_password_store_test_find_login()
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
  pSuite = CU_add_suite("AgsXmlPasswordStoreTest", ags_xml_password_store_test_init_suite, ags_xml_password_store_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsXmlPasswordStore set login name", ags_xml_password_store_test_set_login_name) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlPasswordStore get login name", ags_xml_password_store_test_get_login_name) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlPasswordStore set password", ags_xml_password_store_test_set_password) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlPasswordStore get password", ags_xml_password_store_test_get_password) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlPasswordStore encrypt password", ags_xml_password_store_test_encrypt_password) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlPasswordStore open", ags_xml_password_store_test_open) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlPasswordStore find login", ags_xml_password_store_test_find_login) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
