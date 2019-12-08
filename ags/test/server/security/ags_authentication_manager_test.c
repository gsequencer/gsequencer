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

#define AGS_AUTHENTICATION_MANAGER_TEST_GET_AUTHENTICATION_XML_AUTHENTICATION_COUNT (7)

#define AGS_AUTHENTICATION_MANAGER_TEST_ADD_AUTHENTICATION_XML_AUTHENTICATION_COUNT (9)

#define AGS_AUTHENTICATION_MANAGER_TEST_REMOVE_AUTHENTICATION_XML_AUTHENTICATION_COUNT (13)

#define AGS_AUTHENTICATION_MANAGER_TEST_LOOKUP_LOGIN_DEFAULT_LOGIN "ags-test-login"

#define AGS_AUTHENTICATION_MANAGER_TEST_INSERT_LOGIN_DEFAULT_LOGIN "ags-test-login"

#define AGS_AUTHENTICATION_MANAGER_TEST_REMOVE_LOGIN_DEFAULT_LOGIN "ags-test-login"

#define AGS_AUTHENTICATION_MANAGER_TEST_LOGIN_DEFAULT_AUTHENTICATION_MODULE "ags-xml-authentication"
#define AGS_AUTHENTICATION_MANAGER_TEST_LOGIN_DEFAULT_LOGIN "ags-test-login"
#define AGS_AUTHENTICATION_MANAGER_TEST_LOGIN_DEFAULT_PASSWORD "ags-test-password"

#define AGS_AUTHENTICATION_MANAGER_TEST_LOGOUT_DEFAULT_AUTHENTICATION_MODULE "ags-xml-authentication"
#define AGS_AUTHENTICATION_MANAGER_TEST_LOGOUT_DEFAULT_LOGIN "ags-test-login"
#define AGS_AUTHENTICATION_MANAGER_TEST_LOGOUT_DEFAULT_SECURITY_TOKEN "ags-test-security-token"

#define AGS_AUTHENTICATION_MANAGER_TEST_GET_DIGEST_DEFAULT_AUTHENTICATION_MODULE "ags-xml-authentication"
#define AGS_AUTHENTICATION_MANAGER_TEST_GET_DIGEST_DEFAULT_REALM "ags-test-realm"
#define AGS_AUTHENTICATION_MANAGER_TEST_GET_DIGEST_DEFAULT_LOGIN "ags-test-login"
#define AGS_AUTHENTICATION_MANAGER_TEST_GET_DIGEST_DEFAULT_SECURITY_TOKEN "ags-test-security-token"

#define AGS_AUTHENTICATION_MANAGER_TEST_IS_SESSION_ACTIVE_DEFAULT_LOGIN "ags-test-login"
#define AGS_AUTHENTICATION_MANAGER_TEST_IS_SESSION_ACTIVE_DEFAULT_SECURITY_TOKEN "ags-test-security-token"

AgsServerApplicationContext *server_application_context;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_authentication_manager_test_init_suite()
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
ags_authentication_manager_test_clean_suite()
{
  return(0);
}

void
ags_authentication_manager_test_login_info_alloc()
{
  AgsLoginInfo *login_info;

  login_info = ags_login_info_alloc();

  CU_ASSERT(login_info != 0);
  CU_ASSERT(login_info->ref_count == 1);
  CU_ASSERT(login_info->active_session_count == 0);
  CU_ASSERT(login_info->user_uuid == NULL);
  CU_ASSERT(login_info->security_context == NULL);
}

void
ags_authentication_manager_test_login_info_free()
{
  AgsLoginInfo *login_info;

  /* check empty */
  login_info = ags_login_info_alloc();

  ags_login_info_free(login_info);

  /* check with fields */
  login_info = ags_login_info_alloc();

  login_info->user_uuid = g_strdup("ags-test-uuid");

  login_info->security_context = ags_security_context_new();
  
  ags_login_info_free(login_info);
}

void
ags_authentication_manager_test_login_info_ref()
{
  AgsLoginInfo *login_info;

  /* check empty */
  login_info = ags_login_info_alloc();

  ags_login_info_ref(login_info);
}

void
ags_authentication_manager_test_login_info_unref()
{
  AgsLoginInfo *login_info;

  /* check empty */
  login_info = ags_login_info_alloc();

  ags_login_info_unref(login_info);
}

void
ags_authentication_manager_test_get_authentication()
{
  AgsAuthenticationManager *authentication_manager;
  AgsXmlAuthentication **xml_authentication;

  GList *start_authentication, *authentication;

  guint i;
  
  authentication_manager = ags_authentication_manager_get_instance();

  authentication_manager->authentication = NULL;
  
  /* assert #0 - empty */
  start_authentication = ags_authentication_manager_get_authentication(authentication_manager);

  CU_ASSERT(start_authentication == NULL);

  /* assert #1 - with authentication */
  xml_authentication = (AgsXmlAuthentication **) malloc(AGS_AUTHENTICATION_MANAGER_TEST_GET_AUTHENTICATION_XML_AUTHENTICATION_COUNT * sizeof(AgsXmlAuthentication *));

  for(i = 0; i < AGS_AUTHENTICATION_MANAGER_TEST_GET_AUTHENTICATION_XML_AUTHENTICATION_COUNT; i++){
    xml_authentication[i] = ags_xml_authentication_new();
    authentication_manager->authentication = g_list_prepend(authentication_manager->authentication,
							    xml_authentication[i]);
  }
  
  start_authentication = ags_authentication_manager_get_authentication(authentication_manager);

  CU_ASSERT(start_authentication != NULL);
  CU_ASSERT(g_list_length(start_authentication) == AGS_AUTHENTICATION_MANAGER_TEST_GET_AUTHENTICATION_XML_AUTHENTICATION_COUNT);

}

void
ags_authentication_manager_test_add_authentication()
{
  AgsAuthenticationManager *authentication_manager;
  AgsXmlAuthentication **xml_authentication;

  GList *start_authentication, *authentication;

  guint i;
  
  authentication_manager = ags_authentication_manager_get_instance();

  authentication_manager->authentication = NULL;
  
  /* assert #0 - empty */
  start_authentication = ags_authentication_manager_get_authentication(authentication_manager);

  CU_ASSERT(start_authentication == NULL);

  /* assert #1 - with authentication */
  xml_authentication = (AgsXmlAuthentication **) malloc(AGS_AUTHENTICATION_MANAGER_TEST_ADD_AUTHENTICATION_XML_AUTHENTICATION_COUNT * sizeof(AgsXmlAuthentication *));

  for(i = 0; i < AGS_AUTHENTICATION_MANAGER_TEST_ADD_AUTHENTICATION_XML_AUTHENTICATION_COUNT; i++){
    xml_authentication[i] = ags_xml_authentication_new();
    ags_authentication_manager_add_authentication(authentication_manager,
						  xml_authentication[i]);
  }
  
  start_authentication = ags_authentication_manager_get_authentication(authentication_manager);

  CU_ASSERT(start_authentication != NULL);
  CU_ASSERT(g_list_length(start_authentication) == AGS_AUTHENTICATION_MANAGER_TEST_ADD_AUTHENTICATION_XML_AUTHENTICATION_COUNT);
}

void
ags_authentication_manager_test_remove_authentication()
{
  AgsAuthenticationManager *authentication_manager;
  AgsXmlAuthentication **xml_authentication;

  GList *start_authentication, *authentication;

  guint i;
  
  authentication_manager = ags_authentication_manager_get_instance();

  authentication_manager->authentication = NULL;
  
  /* assert #0 - empty */
  start_authentication = ags_authentication_manager_get_authentication(authentication_manager);

  CU_ASSERT(start_authentication == NULL);

  /* assert #1 - with authentication */
  xml_authentication = (AgsXmlAuthentication **) malloc(AGS_AUTHENTICATION_MANAGER_TEST_REMOVE_AUTHENTICATION_XML_AUTHENTICATION_COUNT * sizeof(AgsXmlAuthentication *));

  for(i = 0; i < AGS_AUTHENTICATION_MANAGER_TEST_REMOVE_AUTHENTICATION_XML_AUTHENTICATION_COUNT; i++){
    xml_authentication[i] = ags_xml_authentication_new();
    authentication_manager->authentication = g_list_prepend(authentication_manager->authentication,
							    xml_authentication[i]);
  }
  
  for(i = 0; i < AGS_AUTHENTICATION_MANAGER_TEST_REMOVE_AUTHENTICATION_XML_AUTHENTICATION_COUNT; i++){
    ags_authentication_manager_remove_authentication(authentication_manager,
						     xml_authentication[i]);
  }

  start_authentication = ags_authentication_manager_get_authentication(authentication_manager);

  CU_ASSERT(start_authentication == NULL);
}

void
ags_authentication_manager_test_get_session_timeout()
{
  AgsAuthenticationManager *authentication_manager;

  gint64 session_timeout;
  
  authentication_manager = ags_authentication_manager_get_instance();

  session_timeout = ags_authentication_manager_get_session_timeout(authentication_manager);

  CU_ASSERT(session_timeout != 0);
}

void
ags_authentication_manager_test_lookup_login()
{
  AgsAuthenticationManager *authentication_manager;

  AgsLoginInfo *login_info;
  
  authentication_manager = ags_authentication_manager_get_instance();

  login_info = ags_authentication_manager_lookup_login(authentication_manager,
						       AGS_AUTHENTICATION_MANAGER_TEST_LOOKUP_LOGIN_DEFAULT_LOGIN);

  CU_ASSERT(login_info == NULL);
}

void
ags_authentication_manager_test_insert_login()
{
  AgsAuthenticationManager *authentication_manager;

  AgsLoginInfo *login_info;
  
  authentication_manager = ags_authentication_manager_get_instance();

  login_info = ags_login_info_alloc();

  ags_authentication_manager_insert_login(authentication_manager,
					  AGS_AUTHENTICATION_MANAGER_TEST_INSERT_LOGIN_DEFAULT_LOGIN,
					  login_info);
}

void
ags_authentication_manager_test_remove_login()
{
  AgsAuthenticationManager *authentication_manager;

  authentication_manager = ags_authentication_manager_get_instance();

  ags_authentication_manager_remove_login(authentication_manager,
					  AGS_AUTHENTICATION_MANAGER_TEST_REMOVE_LOGIN_DEFAULT_LOGIN);
}

void
ags_authentication_manager_test_login()
{
  AgsAuthenticationManager *authentication_manager;

  gchar *user_uuid;
  gchar *security_token;
  
  gboolean success;
  
  authentication_manager = ags_authentication_manager_get_instance();

  success = ags_authentication_manager_login(authentication_manager,
					     AGS_AUTHENTICATION_MANAGER_TEST_LOGIN_DEFAULT_AUTHENTICATION_MODULE,
					     AGS_AUTHENTICATION_MANAGER_TEST_LOGIN_DEFAULT_LOGIN,
					     AGS_AUTHENTICATION_MANAGER_TEST_LOGIN_DEFAULT_PASSWORD,
					     &user_uuid,
					     &security_token);

  CU_ASSERT(success == FALSE);
  CU_ASSERT(user_uuid == NULL);
  CU_ASSERT(security_token == NULL);
}

void
ags_authentication_manager_test_logout()
{
  AgsAuthenticationManager *authentication_manager;
  
  gboolean success;
  
  authentication_manager = ags_authentication_manager_get_instance();

  success = ags_authentication_manager_logout(authentication_manager,
					     AGS_AUTHENTICATION_MANAGER_TEST_LOGOUT_DEFAULT_AUTHENTICATION_MODULE,
					     AGS_AUTHENTICATION_MANAGER_TEST_LOGOUT_DEFAULT_LOGIN,
					     AGS_AUTHENTICATION_MANAGER_TEST_LOGOUT_DEFAULT_SECURITY_TOKEN);

  CU_ASSERT(success == FALSE);
}

void
ags_authentication_manager_test_get_digest()
{
  AgsAuthenticationManager *authentication_manager;
  
  gchar *digest;

  authentication_manager = ags_authentication_manager_get_instance();

  digest = ags_authentication_manager_get_digest(authentication_manager,
						 AGS_AUTHENTICATION_MANAGER_TEST_GET_DIGEST_DEFAULT_AUTHENTICATION_MODULE,
						 AGS_AUTHENTICATION_MANAGER_TEST_GET_DIGEST_DEFAULT_REALM,
						 AGS_AUTHENTICATION_MANAGER_TEST_GET_DIGEST_DEFAULT_LOGIN,
						 AGS_AUTHENTICATION_MANAGER_TEST_GET_DIGEST_DEFAULT_SECURITY_TOKEN);

  CU_ASSERT(digest == NULL);
  
  //TODO:JK: implement me
}

void
ags_authentication_manager_test_is_session_active()
{
  AgsAuthenticationManager *authentication_manager;
  AgsSecurityContext *security_context;
  
  gboolean success;

  authentication_manager = ags_authentication_manager_get_instance();

  security_context = ags_security_context_new();
  
  success = ags_authentication_manager_is_session_active(authentication_manager,
							 security_context,
							 AGS_AUTHENTICATION_MANAGER_TEST_IS_SESSION_ACTIVE_DEFAULT_LOGIN,
							 AGS_AUTHENTICATION_MANAGER_TEST_IS_SESSION_ACTIVE_DEFAULT_SECURITY_TOKEN);

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
