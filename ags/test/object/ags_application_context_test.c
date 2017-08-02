/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2017 Joël Krähemann
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

int ags_application_context_test_init_suite();
int ags_application_context_test_clean_suite();

void ags_application_context_test_load_config();
void ags_application_context_test_register_types();
void ags_application_context_test_add_sibling();
void ags_application_context_test_remove_sibling();
void ags_application_context_test_find_default();
void ags_application_context_test_find_main_loop();
void ags_application_context_test_quit();

void ags_application_context_test_stub_load_config(AgsApplicationContext *application_context);
void ags_application_context_test_stub_register_types(AgsApplicationContext *application_context);
void ags_application_context_test_stub_quit(AgsApplicationContext *application_context);

gboolean stub_load_config = FALSE;
gboolean stub_register_types = FALSE;
gboolean stub_read = FALSE;
gboolean stub_write = FALSE;
gboolean stub_quit = FALSE;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_application_context_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_application_context_test_clean_suite()
{
  return(0);
}

void
ags_application_context_test_load_config()
{
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  /* stub */
  AGS_APPLICATION_CONTEXT_GET_CLASS(application_context)->load_config = ags_application_context_test_stub_load_config;

  /* assert */
  ags_application_context_load_config(application_context);

  CU_ASSERT(stub_load_config == TRUE);
  
  g_object_unref(application_context);
}

void
ags_application_context_test_register_types()
{
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  /* stub */
  AGS_APPLICATION_CONTEXT_GET_CLASS(application_context)->register_types = ags_application_context_test_stub_register_types;

  /* assert */
  ags_application_context_register_types(application_context);

  CU_ASSERT(stub_register_types == TRUE);
  
  g_object_unref(application_context);
}

void
ags_application_context_test_add_sibling()
{
  //TODO:JK: implement me
}

void
ags_application_context_test_remove_sibling()
{
  //TODO:JK: implement me
}

void
ags_application_context_test_find_default()
{
  //TODO:JK: implement me
}

void
ags_application_context_test_find_main_loop()
{
  //TODO:JK: implement me
}

void
ags_application_context_test_quit()
{
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  /* stub */
  AGS_APPLICATION_CONTEXT_GET_CLASS(application_context)->quit = ags_application_context_test_stub_quit;

  /* assert */
  ags_application_context_quit(application_context);

  CU_ASSERT(stub_quit == TRUE);
  
  g_object_unref(application_context);
}

void
ags_application_context_test_stub_load_config(AgsApplicationContext *application_context)
{
  stub_load_config = TRUE;
}

void
ags_application_context_test_stub_register_types(AgsApplicationContext *application_context)
{
  stub_register_types = TRUE;
}

void
ags_application_context_test_stub_quit(AgsApplicationContext *application_context)
{
  stub_quit = TRUE;
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
  pSuite = CU_add_suite("AgsApplicationContextTest\0", ags_application_context_test_init_suite, ags_application_context_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsApplicationContext load config\0", ags_application_context_test_load_config) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplicationContext register types\0", ags_application_context_test_register_types) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplicationContext add sibling\0", ags_application_context_test_add_sibling) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplicationContext remove sibling\0", ags_application_context_test_remove_sibling) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplicationContext find default\0", ags_application_context_test_find_default) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplicationContext find main loop\0", ags_application_context_test_find_main_loop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplicationContext quit\0", ags_application_context_test_quit) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
