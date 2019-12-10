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

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include <ags/libags.h>

int ags_functional_server_test_init_suite();
int ags_functional_server_test_clean_suite();

void ags_functional_server_test_authenticate();

void ags_functional_server_test_authenticate_authenticate_callback(SoupSession *session,
								   SoupMessage *msg,
								   SoupAuth *auth,
								   gboolean retrying,
								   gpointer user_data);

#define AGS_FUNCTIONAL_SERVER_TEST_CONFIG "[server]\n"		\
  "realm=ags-test-realm\n"					\
  "auto-start=true\n"						\
  "any-address=false\n"						\
  "enable-ip4=true\n"						\
  "ip4-address=127.0.0.1\n"					\
  "enable-ip6=false\n"						\
  "ip6-address=::1\n"						\
  "server-port=8080\n"						\
  "\n"						       

#define AGS_FUNCTIONAL_SERVER_TEST_XML_AUTHENTICATION_FILENAME SRCDIR "/" "ags_functional_server_test_authentication.xml"
#define AGS_FUNCTIONAL_SERVER_TEST_XML_PASSWORD_STORE_FILENAME SRCDIR "/" "ags_functional_server_test_password_store.xml"
#define AGS_FUNCTIONAL_SERVER_TEST_XML_COOKIE_FILENAME SRCDIR "/" "ags_functional_server_test_cookie"

#define AGS_FUNCTIONAL_SERVER_TEST_AUTHENTICATE_LOGIN "ags-test-login"
#define AGS_FUNCTIONAL_SERVER_TEST_AUTHENTICATE_PASSWORD "ags-test-password"

AgsServerApplicationContext *server_application_context;

SoupSession *soup_session;
SoupCookieJar *jar;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_server_test_init_suite()
{
  AgsAuthenticationManager *authentication_manager;
  AgsPasswordStoreManager *password_store_manager;
  AgsXmlAuthentication *xml_authentication;
  AgsXmlPasswordStore *xml_password_store;
  
  AgsConfig *config;

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_FUNCTIONAL_SERVER_TEST_CONFIG,
			    strlen(AGS_FUNCTIONAL_SERVER_TEST_CONFIG));

  authentication_manager = ags_authentication_manager_get_instance();
  password_store_manager = ags_password_store_manager_get_instance();

  xml_authentication = ags_xml_authentication_new();
  ags_xml_authentication_open_filename(xml_authentication,
				       AGS_FUNCTIONAL_SERVER_TEST_XML_AUTHENTICATION_FILENAME);
  ags_authentication_manager_add_authentication(authentication_manager,
						xml_authentication);
  
  xml_password_store = ags_xml_password_store_new();
  ags_xml_password_store_open_filename(xml_password_store,
				       AGS_FUNCTIONAL_SERVER_TEST_XML_PASSWORD_STORE_FILENAME);
  ags_password_store_manager_add_password_store(password_store_manager,
						xml_password_store);
  
  server_application_context = (AgsApplicationContext *) ags_server_application_context_new();
  g_object_ref(server_application_context);

  ags_application_context_prepare(server_application_context);
  ags_application_context_setup(server_application_context);
  
  /* server */
  sleep(5);

  /* soup session */
  soup_session = soup_session_new_with_options(SOUP_SESSION_ADD_FEATURE_BY_TYPE, SOUP_TYPE_AUTH_BASIC,
					       NULL);

  jar = soup_cookie_jar_text_new(AGS_FUNCTIONAL_SERVER_TEST_XML_COOKIE_FILENAME,
				 FALSE);     
  soup_session_add_feature(soup_session, jar);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_server_test_clean_suite()
{
  g_object_unref(server_application_context);
  
  return(0);
}

void
ags_functional_server_test_authenticate()
{
  SoupMessage *msg;
  SoupMessageHeaders *response_headers;
  SoupMessageBody *response_body;
  
  SoupMessageHeadersIter iter;
  GSList *cookie;

  gchar *login;
  gchar *security_token;
  char *name, *value;
  
  guint status;

  GError *error;
  
  static const gchar *form_data = "";

  msg = soup_form_request_new("GET",
			      "http://127.0.0.1:8080/ags-xmlrpc",
			      NULL);
  g_signal_connect(soup_session, "authenticate",
		   G_CALLBACK(ags_functional_server_test_authenticate_authenticate_callback), NULL);

  status = soup_session_send_message(soup_session,
				     msg);

  g_object_get(msg,
	       "response-headers", &response_headers,
	       "response-body", &response_body,
	       NULL);

  g_message("status %d", status);
  
  cookie = NULL;
  soup_message_headers_iter_init(&iter,
				 response_headers);

  while(soup_message_headers_iter_next(&iter, &name, &value)){    
    g_message("%s: %s", name, value);
  }
  
  g_message("%s", response_body->data);

  CU_ASSERT(status == 200);

  cookie = soup_cookies_from_response(msg);
  
  login = NULL;
  security_token = NULL;
  
  while(cookie != NULL){
    char *cookie_name;

    cookie_name = soup_cookie_get_name(cookie->data);
    
    if(!g_ascii_strncasecmp(cookie_name,
			    "ags-srv-login",
			    14)){
      login = soup_cookie_get_value(cookie->data);
    }else if(!g_ascii_strncasecmp(cookie_name,
				  "ags-srv-security-token",
				  23)){
      security_token = soup_cookie_get_value(cookie->data);
    }

    if(login != NULL &&
       security_token != NULL){
      break;
    }
    
    cookie = cookie->next;
  }

  CU_ASSERT((!g_strcmp0(login, AGS_FUNCTIONAL_SERVER_TEST_AUTHENTICATE_LOGIN)) == TRUE);
  CU_ASSERT(security_token != NULL);
}

void
ags_functional_server_test_authenticate_authenticate_callback(SoupSession *session,
							      SoupMessage *msg,
							      SoupAuth *auth,
							      gboolean retrying,
							      gpointer user_data)
{
  g_message("authenticate: ****");
  
  soup_auth_authenticate(auth,
			 AGS_FUNCTIONAL_SERVER_TEST_AUTHENTICATE_LOGIN,
			 AGS_FUNCTIONAL_SERVER_TEST_AUTHENTICATE_PASSWORD);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C");
  putenv("LANG=C");

  putenv("LADSPA_PATH=\"\"");
  putenv("DSSI_PATH=\"\"");
  putenv("LV2_PATH=\"\"");

  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFunctionalServerTest", ags_functional_server_test_init_suite, ags_functional_server_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsServer doing authentication", ags_functional_server_test_authenticate) == NULL)){
      CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

