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

int ags_password_store_manager_test_init_suite();
int ags_password_store_manager_test_clean_suite();

void ags_password_store_manager_test_get_password_store();
void ags_password_store_manager_test_add_password_store();
void ags_password_store_manager_test_remove_password_store();
void ags_password_store_manager_test_check_password();

#define AGS_PASSWORD_STORE_MANAGER_TEST_GET_PASSWORD_STORE_XML_PASSWORD_STORE_COUNT (7)

#define AGS_PASSWORD_STORE_MANAGER_TEST_ADD_PASSWORD_STORE_XML_PASSWORD_STORE_COUNT (9)

#define AGS_PASSWORD_STORE_MANAGER_TEST_REMOVE_PASSWORD_STORE_XML_PASSWORD_STORE_COUNT (13)

#define AGS_PASSWORD_STORE_MANAGER_TEST_CHECK_PASSWORD_DEFAULT_LOGIN "ags-test-login"
#define AGS_PASSWORD_STORE_MANAGER_TEST_CHECK_PASSWORD_DEFAULT_PASSWORD "ags-test-password"

AgsServerApplicationContext *server_application_context;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_password_store_manager_test_init_suite()
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
ags_password_store_manager_test_clean_suite()
{
  return(0);
}

void
ags_password_store_manager_test_get_password_store()
{
  AgsPasswordStoreManager *password_store_manager;
  AgsXmlPasswordStore **xml_password_store;

  GList *start_password_store, *password_store;

  guint i;
  
  password_store_manager = ags_password_store_manager_get_instance();

  password_store_manager->password_store = NULL;
  
  /* assert #0 - empty */
  start_password_store = ags_password_store_manager_get_password_store(password_store_manager);

  CU_ASSERT(start_password_store == NULL);

  /* assert #1 - with password store */
  xml_password_store = (AgsXmlPasswordStore **) malloc(AGS_PASSWORD_STORE_MANAGER_TEST_GET_PASSWORD_STORE_XML_PASSWORD_STORE_COUNT * sizeof(AgsXmlPasswordStore *));

  for(i = 0; i < AGS_PASSWORD_STORE_MANAGER_TEST_GET_PASSWORD_STORE_XML_PASSWORD_STORE_COUNT; i++){
    xml_password_store[i] = ags_xml_password_store_new();
    password_store_manager->password_store = g_list_prepend(password_store_manager->password_store,
							    xml_password_store[i]);
  }
  
  start_password_store = ags_password_store_manager_get_password_store(password_store_manager);

  CU_ASSERT(start_password_store != NULL);
  CU_ASSERT(g_list_length(start_password_store) == AGS_PASSWORD_STORE_MANAGER_TEST_GET_PASSWORD_STORE_XML_PASSWORD_STORE_COUNT);
}

void
ags_password_store_manager_test_add_password_store()
{
  AgsPasswordStoreManager *password_store_manager;
  AgsXmlPasswordStore **xml_password_store;

  GList *start_password_store, *password_store;

  guint i;
  
  password_store_manager = ags_password_store_manager_get_instance();

  password_store_manager->password_store = NULL;
  
  /* assert #0 - empty */
  start_password_store = ags_password_store_manager_get_password_store(password_store_manager);

  CU_ASSERT(start_password_store == NULL);

  /* assert #1 - with password store */
  xml_password_store = (AgsXmlPasswordStore **) malloc(AGS_PASSWORD_STORE_MANAGER_TEST_ADD_PASSWORD_STORE_XML_PASSWORD_STORE_COUNT * sizeof(AgsXmlPasswordStore *));

  for(i = 0; i < AGS_PASSWORD_STORE_MANAGER_TEST_ADD_PASSWORD_STORE_XML_PASSWORD_STORE_COUNT; i++){
    xml_password_store[i] = ags_xml_password_store_new();
    ags_password_store_manager_add_password_store(password_store_manager,
						  xml_password_store[i]);
  }
  
  start_password_store = ags_password_store_manager_get_password_store(password_store_manager);

  CU_ASSERT(start_password_store != NULL);
  CU_ASSERT(g_list_length(start_password_store) == AGS_PASSWORD_STORE_MANAGER_TEST_ADD_PASSWORD_STORE_XML_PASSWORD_STORE_COUNT);
}

void
ags_password_store_manager_test_remove_password_store()
{
  AgsPasswordStoreManager *password_store_manager;
  AgsXmlPasswordStore **xml_password_store;

  GList *start_password_store, *password_store;

  guint i;
  
  password_store_manager = ags_password_store_manager_get_instance();

  password_store_manager->password_store = NULL;
  
  /* assert #0 - empty */
  start_password_store = ags_password_store_manager_get_password_store(password_store_manager);

  CU_ASSERT(start_password_store == NULL);

  /* assert #1 - with password store */
  xml_password_store = (AgsXmlPasswordStore **) malloc(AGS_PASSWORD_STORE_MANAGER_TEST_REMOVE_PASSWORD_STORE_XML_PASSWORD_STORE_COUNT * sizeof(AgsXmlPasswordStore *));

  for(i = 0; i < AGS_PASSWORD_STORE_MANAGER_TEST_REMOVE_PASSWORD_STORE_XML_PASSWORD_STORE_COUNT; i++){
    xml_password_store[i] = ags_xml_password_store_new();
    password_store_manager->password_store = g_list_prepend(password_store_manager->password_store,
							    xml_password_store[i]);
  }
  
  for(i = 0; i < AGS_PASSWORD_STORE_MANAGER_TEST_REMOVE_PASSWORD_STORE_XML_PASSWORD_STORE_COUNT; i++){
    ags_password_store_manager_remove_password_store(password_store_manager,
						     xml_password_store[i]);
  }

  start_password_store = ags_password_store_manager_get_password_store(password_store_manager);

  CU_ASSERT(start_password_store == NULL);
}

void
ags_password_store_manager_test_check_password()
{
  AgsPasswordStoreManager *password_store_manager;
  
  gboolean success;
  
  password_store_manager = ags_password_store_manager_get_instance();

  success = ags_password_store_manager_check_password(password_store_manager,
						      AGS_PASSWORD_STORE_MANAGER_TEST_CHECK_PASSWORD_DEFAULT_LOGIN,
						      AGS_PASSWORD_STORE_MANAGER_TEST_CHECK_PASSWORD_DEFAULT_PASSWORD);

  CU_ASSERT(success == FALSE);

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
  pSuite = CU_add_suite("AgsPasswordStoreManagerTest", ags_password_store_manager_test_init_suite, ags_password_store_manager_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsPasswordStoreManager get password store", ags_password_store_manager_test_get_password_store) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPasswordStoreManager add password store", ags_password_store_manager_test_add_password_store) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPasswordStoreManager remove password store", ags_password_store_manager_test_remove_password_store) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPasswordStoreManager check password", ags_password_store_manager_test_check_password) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
