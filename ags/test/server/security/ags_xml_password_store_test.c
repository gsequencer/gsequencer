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
void ags_xml_password_store_test_open_filename();
void ags_xml_password_store_test_find_login();

#define AGS_XML_PASSWORD_STORE_TEST_SET_LOGIN_NAME_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_PASSWORD_STORE_TEST_SET_LOGIN_NAME_DEFAULT_SECURITY_TOKEN "ags-test-security-token"
#define AGS_XML_PASSWORD_STORE_TEST_SET_LOGIN_NAME_DEFAULT_LOGIN_NAME "ags-test-user"

#define AGS_XML_PASSWORD_STORE_TEST_GET_LOGIN_NAME_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_PASSWORD_STORE_TEST_GET_LOGIN_NAME_DEFAULT_SECURITY_TOKEN "ags-test-security-token"

#define AGS_XML_PASSWORD_STORE_TEST_SET_PASSWORD_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_PASSWORD_STORE_TEST_SET_PASSWORD_DEFAULT_SECURITY_TOKEN "ags-test-security-token"
#define AGS_XML_PASSWORD_STORE_TEST_SET_PASSWORD_DEFAULT_PASSWORD "ags-test-password"

#define AGS_XML_PASSWORD_STORE_TEST_GET_PASSWORD_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_PASSWORD_STORE_TEST_GET_PASSWORD_DEFAULT_SECURITY_TOKEN "ags-test-security-token"

#define AGS_XML_PASSWORD_STORE_TEST_ENCRYPT_PASSWORD_DEFAULT_PASSWORD "ags-test-password"
#define AGS_XML_PASSWORD_STORE_TEST_ENCRYPT_PASSWORD_DEFAULT_SALT "ags-puts-salt-in-pizza"

#define AGS_XML_PASSWORD_STORE_TEST_OPEN_FILENAME SRCDIR "/" "ags_password_store_test.xml"

AgsServerApplicationContext *server_application_context;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_xml_password_store_test_init_suite()
{
  AgsConfig *config;

  ags_priority_load_defaults(ags_priority_get_instance());  

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
  AgsSecurityContext *security_context;

  GError *error;
  
  xml_password_store = ags_xml_password_store_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  ags_password_store_set_login_name(AGS_PASSWORD_STORE(xml_password_store),
				    security_context,
				    AGS_XML_PASSWORD_STORE_TEST_SET_LOGIN_NAME_DEFAULT_USER_UUID,
				    AGS_XML_PASSWORD_STORE_TEST_SET_LOGIN_NAME_DEFAULT_SECURITY_TOKEN,
				    AGS_XML_PASSWORD_STORE_TEST_SET_LOGIN_NAME_DEFAULT_LOGIN_NAME,
				    &error);
  
  //TODO:JK: implement me
}

void
ags_xml_password_store_test_get_login_name()
{
  AgsXmlPasswordStore *xml_password_store;
  AgsSecurityContext *security_context;

  gchar *login_name;
  
  GError *error;
  
  xml_password_store = ags_xml_password_store_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  login_name = ags_password_store_get_login_name(AGS_PASSWORD_STORE(xml_password_store),
						 security_context,
						 AGS_XML_PASSWORD_STORE_TEST_SET_LOGIN_NAME_DEFAULT_USER_UUID,
						 AGS_XML_PASSWORD_STORE_TEST_SET_LOGIN_NAME_DEFAULT_SECURITY_TOKEN,
						 &error);

  CU_ASSERT(login_name == NULL);
  
  //TODO:JK: implement me
}

void
ags_xml_password_store_test_set_password()
{
  AgsXmlPasswordStore *xml_password_store;
  AgsSecurityContext *security_context;

  GError *error;
  
  xml_password_store = ags_xml_password_store_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  ags_password_store_set_password(AGS_PASSWORD_STORE(xml_password_store),
				  security_context,
				  AGS_XML_PASSWORD_STORE_TEST_SET_PASSWORD_DEFAULT_USER_UUID,
				  AGS_XML_PASSWORD_STORE_TEST_SET_PASSWORD_DEFAULT_SECURITY_TOKEN,
				  AGS_XML_PASSWORD_STORE_TEST_SET_PASSWORD_DEFAULT_PASSWORD,
				  &error);

  //TODO:JK: implement me
}

void
ags_xml_password_store_test_get_password()
{
  AgsXmlPasswordStore *xml_password_store;
  AgsSecurityContext *security_context;

  gchar *password;
  
  GError *error;
  
  xml_password_store = ags_xml_password_store_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  password = ags_password_store_get_password(AGS_PASSWORD_STORE(xml_password_store),
					     security_context,
					     AGS_XML_PASSWORD_STORE_TEST_SET_PASSWORD_DEFAULT_USER_UUID,
					     AGS_XML_PASSWORD_STORE_TEST_SET_PASSWORD_DEFAULT_SECURITY_TOKEN,
					     &error);

  CU_ASSERT(password == NULL);

  //TODO:JK: implement me
}

void
ags_xml_password_store_test_encrypt_password()
{
  AgsXmlPasswordStore *xml_password_store;
  AgsSecurityContext *security_context;

  gchar *password;
  
  GError *error;
  
  xml_password_store = ags_xml_password_store_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  password = ags_password_store_encrypt_password(AGS_PASSWORD_STORE(xml_password_store),
						 AGS_XML_PASSWORD_STORE_TEST_ENCRYPT_PASSWORD_DEFAULT_PASSWORD,
						 AGS_XML_PASSWORD_STORE_TEST_ENCRYPT_PASSWORD_DEFAULT_SALT,
						 &error);

  CU_ASSERT(password != NULL);
  CU_ASSERT((password != NULL) && ((!g_strcmp0(password, AGS_XML_PASSWORD_STORE_TEST_ENCRYPT_PASSWORD_DEFAULT_PASSWORD)) == FALSE));
  
  //TODO:JK: implement me
}

void
ags_xml_password_store_test_open_filename()
{
  AgsXmlPasswordStore *xml_password_store;
  
  xml_password_store = ags_xml_password_store_new();

  ags_xml_password_store_open_filename(xml_password_store,
				       AGS_XML_PASSWORD_STORE_TEST_OPEN_FILENAME);

  CU_ASSERT(xml_password_store->filename != NULL);
  CU_ASSERT(xml_password_store->doc != NULL);
  CU_ASSERT(xml_password_store->root_node != NULL);
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
     (CU_add_test(pSuite, "test of AgsXmlPasswordStore open filename", ags_xml_password_store_test_open_filename) == NULL) ||
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
