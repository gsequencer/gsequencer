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
void ags_xml_authentication_test_open_filename();
void ags_xml_authentication_test_find_user_uuid();

#define AGS_XML_AUTHENTICATION_TEST_LOGIN_DEFAULT_LOGIN "ags-test-user"
#define AGS_XML_AUTHENTICATION_TEST_LOGIN_DEFAULT_PASSWORD "ags-test-password"

#define AGS_XML_AUTHENTICATION_TEST_LOGOUT_DEFAULT_LOGIN "ags-test-user"
#define AGS_XML_AUTHENTICATION_TEST_LOGOUT_DEFAULT_SECURITY_TOKEN "ags-test-security-token"

#define AGS_XML_AUTHENTICATION_TEST_GET_DIGEST_DEFAULT_REALM "ags-test-realm"
#define AGS_XML_AUTHENTICATION_TEST_GET_DIGEST_DEFAULT_LOGIN "ags-test-user"
#define AGS_XML_AUTHENTICATION_TEST_GET_DIGEST_DEFAULT_SECURITY_TOKEN "ags-test-security-token"

#define AGS_XML_AUTHENTICATION_TEST_IS_SESSION_ACTIVE_DEFAULT_LOGIN "ags-test-user"
#define AGS_XML_AUTHENTICATION_TEST_IS_SESSION_ACTIVE_DEFAULT_SECURITY_TOKEN "ags-test-security-token"

#define AGS_XML_AUTHENTICATION_TEST_OPEN_FILENAME SRCDIR "/" "ags_authentication_test.xml"

AgsServerApplicationContext *server_application_context;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_xml_authentication_test_init_suite()
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
ags_xml_authentication_test_clean_suite()
{
  return(0);
}

void
ags_xml_authentication_test_get_authentication_module()
{
  AgsXmlAuthentication *xml_authentication;

  gchar **authentication_module;
  gchar **iter;

  guint i;
  
  xml_authentication = ags_xml_authentication_new();

  authentication_module = ags_authentication_get_authentication_module(AGS_AUTHENTICATION(xml_authentication));

  CU_ASSERT(authentication_module != NULL);

  i = 0;

  if(authentication_module != NULL){
    iter = authentication_module;

    for(; iter[0] != NULL; i++, iter++);
  }
  
  CU_ASSERT(i > 0);
}

void
ags_xml_authentication_test_login()
{
  AgsXmlAuthentication *xml_authentication;

  gchar *user_uuid, *security_token;

  gboolean success;

  GError *error;
  
  xml_authentication = ags_xml_authentication_new();

  error = NULL;
  success = ags_authentication_login(AGS_AUTHENTICATION(xml_authentication),
				     AGS_XML_AUTHENTICATION_TEST_LOGIN_DEFAULT_LOGIN, AGS_XML_AUTHENTICATION_TEST_LOGIN_DEFAULT_PASSWORD,
				     &user_uuid, &security_token,
				     &error);

  CU_ASSERT(success == FALSE);
  CU_ASSERT(user_uuid == NULL);
  CU_ASSERT(security_token == NULL);
  
  //TODO:JK: implement me
}

void
ags_xml_authentication_test_logout()
{
  AgsXmlAuthentication *xml_authentication;
  AgsSecurityContext *security_context;
  
  gboolean success;

  GError *error;
  
  xml_authentication = ags_xml_authentication_new();

  security_context = ags_auth_security_context_get_instance();
  
  error = NULL;
  success = ags_authentication_logout(AGS_AUTHENTICATION(xml_authentication),
				      security_context,
				      AGS_XML_AUTHENTICATION_TEST_LOGOUT_DEFAULT_LOGIN, AGS_XML_AUTHENTICATION_TEST_LOGOUT_DEFAULT_SECURITY_TOKEN,
				      &error);

  CU_ASSERT(success == FALSE);

  //TODO:JK: implement me
}

void
ags_xml_authentication_test_generate_token()
{
  AgsXmlAuthentication *xml_authentication;
  
  gchar *security_token;

  GError *error;
  
  xml_authentication = ags_xml_authentication_new();

  error = NULL;
  security_token = ags_authentication_generate_token(AGS_AUTHENTICATION(xml_authentication),
						     &error);
  
  CU_ASSERT(security_token != NULL);

  g_free(security_token);
}

void
ags_xml_authentication_test_get_digest()
{
  AgsXmlAuthentication *xml_authentication;
  
  gchar *digest;

  GError *error;
  
  xml_authentication = ags_xml_authentication_new();

  error = NULL;
  digest = ags_authentication_get_digest(AGS_AUTHENTICATION(xml_authentication),
					 AGS_XML_AUTHENTICATION_TEST_GET_DIGEST_DEFAULT_REALM,
					 AGS_XML_AUTHENTICATION_TEST_GET_DIGEST_DEFAULT_LOGIN,
					 AGS_XML_AUTHENTICATION_TEST_GET_DIGEST_DEFAULT_SECURITY_TOKEN,
					 &error);
	
  CU_ASSERT(digest == NULL);
  
  //TODO:JK: implement me
}

void
ags_xml_authentication_test_is_session_active()
{
  AgsXmlAuthentication *xml_authentication;  
  AgsSecurityContext *security_context;
  
  gboolean success;

  GError *error;
  
  xml_authentication = ags_xml_authentication_new();

  security_context = ags_auth_security_context_get_instance();
  
  error = NULL;
  success = ags_authentication_logout(AGS_AUTHENTICATION(xml_authentication),
				      security_context,
				      AGS_XML_AUTHENTICATION_TEST_IS_SESSION_ACTIVE_DEFAULT_LOGIN, AGS_XML_AUTHENTICATION_TEST_IS_SESSION_ACTIVE_DEFAULT_SECURITY_TOKEN,
				      &error);

  CU_ASSERT(success == FALSE);

  //TODO:JK: implement me
}

void
ags_xml_authentication_test_open_filename()
{
  AgsXmlAuthentication *xml_authentication;
  
  xml_authentication = ags_xml_authentication_new();

  ags_xml_authentication_open_filename(xml_authentication,
				       AGS_XML_AUTHENTICATION_TEST_OPEN_FILENAME);

  CU_ASSERT(xml_authentication->filename != NULL);
  CU_ASSERT(xml_authentication->doc != NULL);
  CU_ASSERT(xml_authentication->root_node != NULL);
}

void
ags_xml_authentication_test_find_user_uuid()
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
     (CU_add_test(pSuite, "test of AgsXmlAuthentication is session active", ags_xml_authentication_test_is_session_active) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlAuthentication open filename", ags_xml_authentication_test_open_filename) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlAuthentication find user UUID", ags_xml_authentication_test_find_user_uuid) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
