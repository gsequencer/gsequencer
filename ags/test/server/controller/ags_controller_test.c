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

int ags_controller_test_init_suite();
int ags_controller_test_clean_suite();

void ags_controller_test_resource_alloc();
void ags_controller_test_resource_free();
void ags_controller_test_resource_ref();
void ags_controller_test_resource_unref();
void ags_controller_test_add_resource();
void ags_controller_test_remove_resource();
void ags_controller_test_lookup_resource();
void ags_controller_test_query_security_context();

#define AGS_CONTROLLER_TEST_ADD_RESOURCE_RESOURCE_COUNT (7)

#define AGS_CONTROLLER_TEST_REMOVE_RESOURCE_RESOURCE_COUNT (9)

#define AGS_CONTROLLER_TEST_LOOKUP_RESOURCE_RESOURCE_COUNT (5)

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_controller_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_controller_test_clean_suite()
{
  return(0);
}

void
ags_controller_test_resource_alloc()
{
  AgsControllerResource *controller_resource;

  controller_resource = ags_controller_resource_alloc("group-id", "user-id",
						      0x744);

  CU_ASSERT(controller_resource != NULL);
  CU_ASSERT(controller_resource->ref_count == 1);
  CU_ASSERT(!g_strcmp0(controller_resource->group_id, "group-id") == TRUE);
  CU_ASSERT(!g_strcmp0(controller_resource->user_id, "user-id") == TRUE);
  CU_ASSERT(controller_resource->access_mode == 0x744);
}

void
ags_controller_test_resource_free()
{
  AgsControllerResource *controller_resource;

  controller_resource = ags_controller_resource_alloc(NULL, NULL,
						      0x0);

  ags_controller_resource_free(controller_resource);
}

void
ags_controller_test_resource_ref()
{
  AgsControllerResource *controller_resource;

  controller_resource = ags_controller_resource_alloc("group-id", "user-id",
						      0x744);

  ags_controller_resource_ref(controller_resource);
  CU_ASSERT(controller_resource->ref_count == 2);
}

void
ags_controller_test_resource_unref()
{
  AgsControllerResource *controller_resource;

  controller_resource = ags_controller_resource_alloc("group-id", "user-id",
						      0x744);

  ags_controller_resource_unref(controller_resource);
}

void
ags_controller_test_add_resource()
{
  AgsController *controller;
  
  AgsControllerResource **controller_resource;

  guint i;
  
  controller = ags_controller_new();

  controller_resource = (AgsControllerResource **) malloc(AGS_CONTROLLER_TEST_ADD_RESOURCE_RESOURCE_COUNT * sizeof(AgsControllerResource *));

  for(i = 0; i < AGS_CONTROLLER_TEST_ADD_RESOURCE_RESOURCE_COUNT; i++){
    controller_resource[i] = ags_controller_resource_alloc(NULL, NULL,
							   0x0);
    ags_controller_add_resource(controller,
				g_strdup_printf("resource-%d", i),
				controller_resource[i]);
				
  }

  CU_ASSERT(g_hash_table_size(controller->resource) == AGS_CONTROLLER_TEST_ADD_RESOURCE_RESOURCE_COUNT);
}

void
ags_controller_test_remove_resource()
{
  AgsController *controller;
  
  AgsControllerResource **controller_resource;

  guint i;
  
  controller = ags_controller_new();

  controller_resource = (AgsControllerResource **) malloc(AGS_CONTROLLER_TEST_REMOVE_RESOURCE_RESOURCE_COUNT * sizeof(AgsControllerResource *));

  for(i = 0; i < AGS_CONTROLLER_TEST_REMOVE_RESOURCE_RESOURCE_COUNT; i++){
    controller_resource[i] = ags_controller_resource_alloc(NULL, NULL,
							   0x0);
    g_hash_table_insert(controller->resource,
			g_strdup_printf("resource-%d", i),
			controller_resource[i]);
				
  }

  for(i = 0; i < AGS_CONTROLLER_TEST_REMOVE_RESOURCE_RESOURCE_COUNT; i++){
    ags_controller_remove_resource(controller,
				   g_strdup_printf("resource-%d", i));
				   
  }
      
  CU_ASSERT(g_hash_table_size(controller->resource) == 0);
}

void
ags_controller_test_lookup_resource()
{
  AgsController *controller;
  
  AgsControllerResource **controller_resource;

  guint i;
  gboolean success;
  
  controller = ags_controller_new();

  controller_resource = (AgsControllerResource **) malloc(AGS_CONTROLLER_TEST_LOOKUP_RESOURCE_RESOURCE_COUNT * sizeof(AgsControllerResource *));

  for(i = 0; i < AGS_CONTROLLER_TEST_LOOKUP_RESOURCE_RESOURCE_COUNT; i++){
    controller_resource[i] = ags_controller_resource_alloc(NULL, NULL,
							   0x0);
    g_hash_table_insert(controller->resource,
			g_strdup_printf("resource-%d", i),
			controller_resource[i]);
				
  }

  success = TRUE;

  for(i = 0; i < AGS_CONTROLLER_TEST_LOOKUP_RESOURCE_RESOURCE_COUNT; i++){
    AgsControllerResource *current;
    
    current = ags_controller_lookup_resource(controller,
					     g_strdup_printf("resource-%d", i));

    if(current == NULL){
      success = FALSE;
    }
  }

  CU_ASSERT(success == TRUE);
}

void
ags_controller_test_query_security_context()
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
  pSuite = CU_add_suite("AgsControllerTest", ags_controller_test_init_suite, ags_controller_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsController resource alloc", ags_controller_test_resource_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of AgsController resource free", ags_controller_test_resource_free) == NULL) ||
     (CU_add_test(pSuite, "test of AgsController resource ref", ags_controller_test_resource_ref) == NULL) ||
     (CU_add_test(pSuite, "test of AgsController resource unref", ags_controller_test_resource_unref) == NULL) ||
     (CU_add_test(pSuite, "test of AgsController add resource", ags_controller_test_add_resource) == NULL) ||
     (CU_add_test(pSuite, "test of AgsController remove resource", ags_controller_test_remove_resource) == NULL) ||
     (CU_add_test(pSuite, "test of AgsController lookup resource", ags_controller_test_lookup_resource) == NULL) ||
     (CU_add_test(pSuite, "test of AgsController query security context", ags_controller_test_query_security_context) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
