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

int ags_business_group_manager_test_init_suite();
int ags_business_group_manager_test_clean_suite();

void ags_business_group_manager_test_get_business_group();
void ags_business_group_manager_test_add_business_group();
void ags_business_group_manager_test_remove_business_group();

#define AGS_BUSINESS_GROUP_MANAGER_TEST_GET_BUSINESS_GROUP_XML_BUSINESS_GROUP_COUNT (7)

#define AGS_BUSINESS_GROUP_MANAGER_TEST_ADD_BUSINESS_GROUP_XML_BUSINESS_GROUP_COUNT (9)

#define AGS_BUSINESS_GROUP_MANAGER_TEST_REMOVE_BUSINESS_GROUP_XML_BUSINESS_GROUP_COUNT (13)

AgsServerApplicationContext *server_application_context;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_business_group_manager_test_init_suite()
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
ags_business_group_manager_test_clean_suite()
{
  return(0);
}

void
ags_business_group_manager_test_get_business_group()
{
  AgsBusinessGroupManager *business_group_manager;
  AgsXmlBusinessGroup **xml_business_group;

  GList *start_business_group, *business_group;

  guint i;
  
  business_group_manager = ags_business_group_manager_get_instance();

  business_group_manager->business_group = NULL;
  
  /* assert #0 - empty */
  start_business_group = ags_business_group_manager_get_business_group(business_group_manager);

  CU_ASSERT(start_business_group == NULL);

  /* assert #1 - with business group */
  xml_business_group = (AgsXmlBusinessGroup **) malloc(AGS_BUSINESS_GROUP_MANAGER_TEST_GET_BUSINESS_GROUP_XML_BUSINESS_GROUP_COUNT * sizeof(AgsXmlBusinessGroup *));

  for(i = 0; i < AGS_BUSINESS_GROUP_MANAGER_TEST_GET_BUSINESS_GROUP_XML_BUSINESS_GROUP_COUNT; i++){
    xml_business_group[i] = ags_xml_business_group_new();
    business_group_manager->business_group = g_list_prepend(business_group_manager->business_group,
							    xml_business_group[i]);
  }
  
  start_business_group = ags_business_group_manager_get_business_group(business_group_manager);

  CU_ASSERT(start_business_group != NULL);
  CU_ASSERT(g_list_length(start_business_group) == AGS_BUSINESS_GROUP_MANAGER_TEST_GET_BUSINESS_GROUP_XML_BUSINESS_GROUP_COUNT);
}

void
ags_business_group_manager_test_add_business_group()
{
  AgsBusinessGroupManager *business_group_manager;
  AgsXmlBusinessGroup **xml_business_group;

  GList *start_business_group, *business_group;

  guint i;
  
  business_group_manager = ags_business_group_manager_get_instance();

  business_group_manager->business_group = NULL;
  
  /* assert #0 - empty */
  start_business_group = ags_business_group_manager_get_business_group(business_group_manager);

  CU_ASSERT(start_business_group == NULL);

  /* assert #1 - with business group */
  xml_business_group = (AgsXmlBusinessGroup **) malloc(AGS_BUSINESS_GROUP_MANAGER_TEST_ADD_BUSINESS_GROUP_XML_BUSINESS_GROUP_COUNT * sizeof(AgsXmlBusinessGroup *));

  for(i = 0; i < AGS_BUSINESS_GROUP_MANAGER_TEST_ADD_BUSINESS_GROUP_XML_BUSINESS_GROUP_COUNT; i++){
    xml_business_group[i] = ags_xml_business_group_new();
    ags_business_group_manager_add_business_group(business_group_manager,
						  xml_business_group[i]);
  }
  
  start_business_group = ags_business_group_manager_get_business_group(business_group_manager);

  CU_ASSERT(start_business_group != NULL);
  CU_ASSERT(g_list_length(start_business_group) == AGS_BUSINESS_GROUP_MANAGER_TEST_ADD_BUSINESS_GROUP_XML_BUSINESS_GROUP_COUNT);
}

void
ags_business_group_manager_test_remove_business_group()
{
  AgsBusinessGroupManager *business_group_manager;
  AgsXmlBusinessGroup **xml_business_group;

  GList *start_business_group, *business_group;

  guint i;
  
  business_group_manager = ags_business_group_manager_get_instance();

  business_group_manager->business_group = NULL;
  
  /* assert #0 - empty */
  start_business_group = ags_business_group_manager_get_business_group(business_group_manager);

  CU_ASSERT(start_business_group == NULL);

  /* assert #1 - with business group */
  xml_business_group = (AgsXmlBusinessGroup **) malloc(AGS_BUSINESS_GROUP_MANAGER_TEST_REMOVE_BUSINESS_GROUP_XML_BUSINESS_GROUP_COUNT * sizeof(AgsXmlBusinessGroup *));

  for(i = 0; i < AGS_BUSINESS_GROUP_MANAGER_TEST_REMOVE_BUSINESS_GROUP_XML_BUSINESS_GROUP_COUNT; i++){
    xml_business_group[i] = ags_xml_business_group_new();
    business_group_manager->business_group = g_list_prepend(business_group_manager->business_group,
							    xml_business_group[i]);
  }
  
  for(i = 0; i < AGS_BUSINESS_GROUP_MANAGER_TEST_REMOVE_BUSINESS_GROUP_XML_BUSINESS_GROUP_COUNT; i++){
    ags_business_group_manager_remove_business_group(business_group_manager,
						     xml_business_group[i]);
  }

  start_business_group = ags_business_group_manager_get_business_group(business_group_manager);

  CU_ASSERT(start_business_group == NULL);
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
  pSuite = CU_add_suite("AgsBusinessGroupManagerTest", ags_business_group_manager_test_init_suite, ags_business_group_manager_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsBusinessGroupManager get business group", ags_business_group_manager_test_get_business_group) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBusinessGroupManager add business group", ags_business_group_manager_test_add_business_group) == NULL) ||
     (CU_add_test(pSuite, "test of AgsBusinessGroupManager remove business group", ags_business_group_manager_test_remove_business_group) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
