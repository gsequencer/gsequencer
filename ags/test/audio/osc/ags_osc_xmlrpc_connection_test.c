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
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int ags_osc_xmlrpc_connection_test_init_suite();
int ags_osc_xmlrpc_connection_test_clean_suite();

void ags_osc_xmlrpc_connection_test_set_property();
void ags_osc_xmlrpc_connection_test_get_property();

#define AGS_OSC_XMLRPC_CONNECTION_TEST_SET_PROPERTY_PATH "/ags-test-path"
#define AGS_OSC_XMLRPC_CONNECTION_TEST_SET_PROPERTY_LOGIN "ags-test-login"
#define AGS_OSC_XMLRPC_CONNECTION_TEST_SET_PROPERTY_SECURITY_TOKEN "ags-test-security-token"

#define AGS_OSC_XMLRPC_CONNECTION_TEST_GET_PROPERTY_PATH "/ags-test-path"
#define AGS_OSC_XMLRPC_CONNECTION_TEST_GET_PROPERTY_LOGIN "ags-test-login"
#define AGS_OSC_XMLRPC_CONNECTION_TEST_GET_PROPERTY_SECURITY_TOKEN "ags-test-security-token"

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_xmlrpc_connection_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_xmlrpc_connection_test_clean_suite()
{
  return(0);
}

void
ags_osc_xmlrpc_connection_test_set_property()
{
  AgsOscXmlrpcConnection *osc_xmlrpc_connection;

  AgsSecurityContext *security_context;
  
  osc_xmlrpc_connection = ags_osc_xmlrpc_connection_new(NULL);

  security_context = ags_security_context_new();

  g_object_set(osc_xmlrpc_connection,
	       "security-context", security_context,
	       "path", AGS_OSC_XMLRPC_CONNECTION_TEST_SET_PROPERTY_PATH,
	       "login", AGS_OSC_XMLRPC_CONNECTION_TEST_SET_PROPERTY_LOGIN,
	       "security-token", AGS_OSC_XMLRPC_CONNECTION_TEST_SET_PROPERTY_SECURITY_TOKEN,
	       NULL);

  CU_ASSERT(osc_xmlrpc_connection->security_context == security_context);

  CU_ASSERT((!g_strcmp0(osc_xmlrpc_connection->path, AGS_OSC_XMLRPC_CONNECTION_TEST_SET_PROPERTY_PATH)) == TRUE);
  CU_ASSERT((!g_strcmp0(osc_xmlrpc_connection->login, AGS_OSC_XMLRPC_CONNECTION_TEST_SET_PROPERTY_LOGIN)) == TRUE);
  CU_ASSERT((!g_strcmp0(osc_xmlrpc_connection->security_token, AGS_OSC_XMLRPC_CONNECTION_TEST_SET_PROPERTY_SECURITY_TOKEN)) == TRUE);
}

void
ags_osc_xmlrpc_connection_test_get_property()
{
  AgsOscXmlrpcConnection *osc_xmlrpc_connection;

  AgsSecurityContext *security_context;

  gchar *path;
  gchar *login;
  gchar *security_token;
  
  osc_xmlrpc_connection = ags_osc_xmlrpc_connection_new(NULL);

  osc_xmlrpc_connection->security_context = ags_security_context_new();

  osc_xmlrpc_connection->path = AGS_OSC_XMLRPC_CONNECTION_TEST_GET_PROPERTY_PATH;
  osc_xmlrpc_connection->login = AGS_OSC_XMLRPC_CONNECTION_TEST_GET_PROPERTY_LOGIN;
  osc_xmlrpc_connection->security_token = AGS_OSC_XMLRPC_CONNECTION_TEST_GET_PROPERTY_SECURITY_TOKEN;
  
  g_object_get(osc_xmlrpc_connection,
	       "security-context", &security_context,
	       "path", &path,
	       "login", &login,
	       "security-token", &security_token,
	       NULL);

  CU_ASSERT(osc_xmlrpc_connection->security_context == security_context);

  CU_ASSERT((!g_strcmp0(path, AGS_OSC_XMLRPC_CONNECTION_TEST_GET_PROPERTY_PATH)) == TRUE);
  CU_ASSERT((!g_strcmp0(login, AGS_OSC_XMLRPC_CONNECTION_TEST_GET_PROPERTY_LOGIN)) == TRUE);
  CU_ASSERT((!g_strcmp0(security_token, AGS_OSC_XMLRPC_CONNECTION_TEST_GET_PROPERTY_SECURITY_TOKEN)) == TRUE);
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
  pSuite = CU_add_suite("AgsOscXmlrpcConnectionTest", ags_osc_xmlrpc_connection_test_init_suite, ags_osc_xmlrpc_connection_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsOscXmlrpcConnection set property", ags_osc_xmlrpc_connection_test_set_property) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscXmlrpcConnection get property", ags_osc_xmlrpc_connection_test_get_property) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
