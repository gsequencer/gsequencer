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

int ags_security_context_test_init_suite();
int ags_security_context_test_clean_suite();

void ags_security_context_test_parse_business_group();
void ags_security_context_test_add_server_context();
void ags_security_context_test_remove_server_context();

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_security_context_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_security_context_test_clean_suite()
{
  return(0);
}

void
ags_security_context_test_parse_business_group()
{
  //TODO:JK: implement me
}

void
ags_security_context_test_add_server_context()
{
  AgsSecurityContext *security_context;

  security_context = ags_security_context_new();

  ags_security_context_add_server_context(security_context,
					  "/ags-xmlrpc/logout");

  CU_ASSERT(g_strv_contains(security_context->server_context, "/ags-xmlrpc/logout") == TRUE);
  
  ags_security_context_add_server_context(security_context,
					  "/ags-xmlrpc/osc");

  CU_ASSERT(g_strv_contains(security_context->server_context, "/ags-xmlrpc/logout") == TRUE);
  CU_ASSERT(g_strv_contains(security_context->server_context, "/ags-xmlrpc/osc") == TRUE);
}

void
ags_security_context_test_remove_server_context()
{
  AgsSecurityContext *security_context;

  security_context = ags_security_context_new();

  security_context->server_context = (gchar **) malloc(3 * sizeof(gchar *));

  security_context->server_context[0] = g_strdup("/ags-xmlrpc/logout");
  security_context->server_context[1] = g_strdup("/ags-xmlrpc/osc");
  security_context->server_context[2] = NULL;

  /* remove #0 */
  ags_security_context_remove_server_context(security_context, "/ags-xmlrpc/logout");
  
  CU_ASSERT(g_strv_contains(security_context->server_context, "/ags-xmlrpc/logout") == FALSE);
  CU_ASSERT(g_strv_contains(security_context->server_context, "/ags-xmlrpc/osc") == TRUE);

  /* remove #1 */
  ags_security_context_remove_server_context(security_context, "/ags-xmlrpc/osc");
  
  CU_ASSERT(security_context->server_context == NULL);
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
  pSuite = CU_add_suite("AgsSecurityContextTest", ags_security_context_test_init_suite, ags_security_context_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsSecurityContext parse business group", ags_security_context_test_parse_business_group) == NULL) ||
    (CU_add_test(pSuite, "test of AgsSecurityContext add server context", ags_security_context_test_add_server_context) == NULL) ||
    (CU_add_test(pSuite, "test of AgsSecurityContext remove server context", ags_security_context_test_remove_server_context) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
