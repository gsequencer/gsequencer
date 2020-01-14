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

int ags_server_application_context_test_init_suite();
int ags_server_application_context_test_clean_suite();

void ags_server_application_context_test_dispose();
void ags_server_application_context_test_finalize();

void ags_server_application_context_test_finalize_stub(GObject *gobject);

gboolean server_application_context_test_finalized;

extern AgsApplicationContext *ags_application_context;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_server_application_context_test_init_suite()
{    
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_server_application_context_test_clean_suite()
{
  
  return(0);
}

void
ags_server_application_context_test_dispose()
{
  AgsServerApplicationContext *server_application_context;

  AgsConfig *config;

  config = ags_config_get_instance();
//  ags_config_load_from_data(config,
//			    AGS_SERVER_APPLICATION_CONTEXT_TEST_CONFIG,
//			    strlen(AGS_SERVER_APPLICATION_CONTEXT_TEST_CONFIG));

  server_application_context = g_object_new(AGS_TYPE_SERVER_APPLICATION_CONTEXT,
					    NULL);
  g_object_ref(server_application_context);
  
  ags_application_context_prepare(server_application_context);
  ags_application_context_setup(server_application_context);

  /* run dispose */
  g_object_run_dispose(server_application_context);

  /* assert */
  CU_ASSERT(server_application_context->thread_pool == NULL);
  CU_ASSERT(server_application_context->worker == NULL);
  CU_ASSERT(server_application_context->registry == NULL);
  CU_ASSERT(server_application_context->server == NULL);

  ags_application_context = NULL;
}

void
ags_server_application_context_test_finalize()
{
  AgsServerApplicationContext *server_application_context;

  AgsConfig *config;

  config = ags_config_get_instance();
//  ags_config_load_from_data(config,
//			    AGS_SERVER_APPLICATION_CONTEXT_TEST_CONFIG,
//			    strlen(AGS_SERVER_APPLICATION_CONTEXT_TEST_CONFIG));

  server_application_context = g_object_new(AGS_TYPE_SERVER_APPLICATION_CONTEXT,
					    NULL);

  ags_application_context_prepare(server_application_context);
  ags_application_context_setup(server_application_context);

  /* run dispose */
  g_object_run_dispose(server_application_context);

  /* stub finalize */
  server_application_context_test_finalized = FALSE;
  G_OBJECT_GET_CLASS(server_application_context)->finalize = ags_server_application_context_test_finalize_stub;

  /* unref and assert */
  g_object_unref(server_application_context);
  
  CU_ASSERT(server_application_context_test_finalized == TRUE);

  ags_application_context = NULL;
}

void
ags_server_application_context_test_finalize_stub(GObject *gobject)
{
  server_application_context_test_finalized = TRUE;
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
  pSuite = CU_add_suite("AgsServerApplicationContextTest", ags_server_application_context_test_init_suite, ags_server_application_context_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsServerApplicationContext doing dispose", ags_server_application_context_test_dispose) == NULL) ||
     (CU_add_test(pSuite, "test of AgsServerApplicationContext doing finalize", ags_server_application_context_test_finalize) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
