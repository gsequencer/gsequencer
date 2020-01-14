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

int ags_osc_xmlrpc_message_test_init_suite();
int ags_osc_xmlrpc_message_test_clean_suite();

void ags_osc_xmlrpc_message_test_set_property();
void ags_osc_xmlrpc_message_test_get_property();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_xmlrpc_message_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_osc_xmlrpc_message_test_clean_suite()
{
  return(0);
}

void
ags_osc_xmlrpc_message_test_set_property()
{
  AgsOscXmlrpcMessage *osc_xmlrpc_message;

  SoupMessage *soup_message;
  GHashTable *query;
  
  osc_xmlrpc_message = ags_osc_xmlrpc_message_new();

  soup_message = soup_message_new("/ags-test-osc-xmlrpc",
				  "http://localhost/ags-test-osc-xmlrpc");

  query = g_hash_table_new(g_str_hash,
			   g_str_equal);

  g_object_set(osc_xmlrpc_message,
	       "msg", soup_message,
	       "query", query,
	       NULL);

  CU_ASSERT(osc_xmlrpc_message->msg == soup_message);
  CU_ASSERT(osc_xmlrpc_message->query == query);
}

void
ags_osc_xmlrpc_message_test_get_property()
{
  AgsOscXmlrpcMessage *osc_xmlrpc_message;

  SoupMessage *soup_message;
  GHashTable *query;

  osc_xmlrpc_message = ags_osc_xmlrpc_message_new();

  osc_xmlrpc_message->msg = soup_message_new("/ags-test-osc-xmlrpc",
					     "http://localhost/ags-test-osc-xmlrpc");

  osc_xmlrpc_message->query = g_hash_table_new(g_str_hash,
					       g_str_equal);

  g_object_get(osc_xmlrpc_message,
	       "msg", &soup_message,
	       "query", &query,
	       NULL);

  CU_ASSERT(osc_xmlrpc_message->msg == soup_message);
  CU_ASSERT(osc_xmlrpc_message->query == query);
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
  pSuite = CU_add_suite("AgsOscXmlrpcMessageTest", ags_osc_xmlrpc_message_test_init_suite, ags_osc_xmlrpc_message_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsOscXmlrpcMessage set property", ags_osc_xmlrpc_message_test_set_property) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscXmlrpcMessage get property", ags_osc_xmlrpc_message_test_get_property) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
