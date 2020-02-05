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

int ags_front_controller_test_init_suite();
int ags_front_controller_test_clean_suite();

void ags_front_controller_test_do_request();

AgsServerApplicationContext *server_application_context;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_front_controller_test_init_suite()
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
ags_front_controller_test_clean_suite()
{
  return(0);
}

void
ags_front_controller_test_do_request()
{
  AgsServer *server;
  AgsFrontController *front_controller;
  AgsSecurityContext *security_context;
  
  gpointer response;

  server = server_application_context->server->data;
  
  front_controller = ags_front_controller_new();
  ags_server_add_controller(server,
			    front_controller);
  g_object_set(front_controller,
	       "server", server,
	       NULL);
  
  security_context = ags_security_context_new();
  
  response = ags_front_controller_do_request(front_controller,
					     NULL,
					     NULL,
					     NULL,
					     security_context,
					     "/ags-xmlrpc/status",
					     "ags-test-login",
					     "ags-test-security-token");
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
  pSuite = CU_add_suite("AgsFrontControllerTest", ags_front_controller_test_init_suite, ags_front_controller_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFrontController do request", ags_front_controller_test_do_request) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
