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

int ags_xml_business_group_test_init_suite();
int ags_xml_business_group_test_clean_suite();

void ags_xml_business_group_test_get_group_uuid();
void ags_xml_business_group_test_set_group_name();
void ags_xml_business_group_test_get_group_name();
void ags_xml_business_group_test_set_user();
void ags_xml_business_group_test_get_user();
void ags_xml_business_group_test_open_filename();

#define AGS_XML_BUSINESS_GROUP_TEST_GET_GROUP_UUID_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_BUSINESS_GROUP_TEST_GET_GROUP_UUID_DEFAULT_SECURITY_TOKEN "ags-test-security-token"

#define AGS_XML_BUSINESS_GROUP_TEST_SET_GROUP_NAME_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_BUSINESS_GROUP_TEST_SET_GROUP_NAME_DEFAULT_SECURITY_TOKEN "ags-test-security-token"
#define AGS_XML_BUSINESS_GROUP_TEST_SET_GROUP_NAME_DEFAULT_GROUP_UUID "ags-test-group-uuid"
#define AGS_XML_BUSINESS_GROUP_TEST_SET_GROUP_NAME_DEFAULT_GROUP_NAME "ags-test-group-name"

#define AGS_XML_BUSINESS_GROUP_TEST_GET_GROUP_NAME_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_BUSINESS_GROUP_TEST_GET_GROUP_NAME_DEFAULT_SECURITY_TOKEN "ags-test-security-token"
#define AGS_XML_BUSINESS_GROUP_TEST_GET_GROUP_NAME_DEFAULT_GROUP_UUID "ags-test-group-uuid"

#define AGS_XML_BUSINESS_GROUP_TEST_SET_USER_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_BUSINESS_GROUP_TEST_SET_USER_DEFAULT_SECURITY_TOKEN "ags-test-security-token"
#define AGS_XML_BUSINESS_GROUP_TEST_SET_USER_DEFAULT_GROUP_UUID "ags-test-group-uuid"

#define AGS_XML_BUSINESS_GROUP_TEST_GET_USER_DEFAULT_USER_UUID "ags-test-uuid"
#define AGS_XML_BUSINESS_GROUP_TEST_GET_USER_DEFAULT_SECURITY_TOKEN "ags-test-security-token"
#define AGS_XML_BUSINESS_GROUP_TEST_GET_USER_DEFAULT_GROUP_UUID "ags-test-group-uuid"

#define AGS_XML_BUSINESS_GROUP_TEST_OPEN_FILENAME AGS_SRC_DIR "/" "ags_business_group_test.xml"

AgsServerApplicationContext *server_application_context;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_xml_business_group_test_init_suite()
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
ags_xml_business_group_test_clean_suite()
{
  return(0);
}

void
ags_xml_business_group_test_get_group_uuid()
{
  AgsXmlBusinessGroup *xml_business_group;
  AgsSecurityContext *security_context;

  GError *error;
  
  xml_business_group = ags_xml_business_group_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  ags_business_group_get_group_uuid(AGS_BUSINESS_GROUP(xml_business_group),
				    security_context,
				    AGS_XML_BUSINESS_GROUP_TEST_GET_GROUP_UUID_DEFAULT_USER_UUID,
				    AGS_XML_BUSINESS_GROUP_TEST_GET_GROUP_UUID_DEFAULT_SECURITY_TOKEN,
				    &error);
  
  //TODO:JK: implement me
}

void
ags_xml_business_group_test_set_group_name()
{
  AgsXmlBusinessGroup *xml_business_group;
  AgsSecurityContext *security_context;

  GError *error;
  
  xml_business_group = ags_xml_business_group_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  ags_business_group_set_group_name(AGS_BUSINESS_GROUP(xml_business_group),
				    security_context,
				    AGS_XML_BUSINESS_GROUP_TEST_SET_GROUP_NAME_DEFAULT_USER_UUID,
				    AGS_XML_BUSINESS_GROUP_TEST_SET_GROUP_NAME_DEFAULT_SECURITY_TOKEN,
				    AGS_XML_BUSINESS_GROUP_TEST_SET_GROUP_NAME_DEFAULT_GROUP_UUID,
				    AGS_XML_BUSINESS_GROUP_TEST_SET_GROUP_NAME_DEFAULT_GROUP_NAME,
				    &error);
  
  //TODO:JK: implement me
}

void
ags_xml_business_group_test_get_group_name()
{
  AgsXmlBusinessGroup *xml_business_group;
  AgsSecurityContext *security_context;

  gchar *group_name;
  
  GError *error;
  
  xml_business_group = ags_xml_business_group_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  group_name = ags_business_group_get_group_name(AGS_BUSINESS_GROUP(xml_business_group),
						 security_context,
						 AGS_XML_BUSINESS_GROUP_TEST_GET_GROUP_NAME_DEFAULT_USER_UUID,
						 AGS_XML_BUSINESS_GROUP_TEST_GET_GROUP_NAME_DEFAULT_SECURITY_TOKEN,
						 AGS_XML_BUSINESS_GROUP_TEST_GET_GROUP_NAME_DEFAULT_GROUP_UUID,
						 &error);

  CU_ASSERT(group_name == NULL);
  
  //TODO:JK: implement me
}

void
ags_xml_business_group_test_set_user()
{
  AgsXmlBusinessGroup *xml_business_group;
  AgsSecurityContext *security_context;

  GError *error;

  static const gchar **default_user = {
    "ags-test-user-uuid-0",
    "ags-test-user-uuid-1",
    "ags-test-user-uuid-2",
    NULL,
  };
  
  xml_business_group = ags_xml_business_group_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  ags_business_group_set_user(AGS_BUSINESS_GROUP(xml_business_group),
			      security_context,
			      AGS_XML_BUSINESS_GROUP_TEST_SET_USER_DEFAULT_USER_UUID,
			      AGS_XML_BUSINESS_GROUP_TEST_SET_USER_DEFAULT_SECURITY_TOKEN,
			      AGS_XML_BUSINESS_GROUP_TEST_SET_USER_DEFAULT_GROUP_UUID,
			      default_user,
			      &error);
  
  //TODO:JK: implement me
}

void
ags_xml_business_group_test_get_user()
{
  AgsXmlBusinessGroup *xml_business_group;
  AgsSecurityContext *security_context;

  gchar *user;
  
  GError *error;
  
  xml_business_group = ags_xml_business_group_new();

  security_context = ags_auth_security_context_get_instance();

  error = NULL;
  user = ags_business_group_get_user(AGS_BUSINESS_GROUP(xml_business_group),
				     security_context,
				     AGS_XML_BUSINESS_GROUP_TEST_GET_USER_DEFAULT_USER_UUID,
				     AGS_XML_BUSINESS_GROUP_TEST_GET_USER_DEFAULT_SECURITY_TOKEN,
				     AGS_XML_BUSINESS_GROUP_TEST_GET_USER_DEFAULT_GROUP_UUID,
				     &error);

  CU_ASSERT(user == NULL);
  
  //TODO:JK: implement me
}

void
ags_xml_business_group_test_open_filename()
{
  AgsXmlPasswordStore *xml_business_group;
  
  xml_business_group = ags_xml_business_group_new();

  ags_xml_business_group_open_filename(xml_business_group,
				       AGS_XML_BUSINESS_GROUP_TEST_OPEN_FILENAME);

  CU_ASSERT(xml_business_group->filename != NULL);
  CU_ASSERT(xml_business_group->doc != NULL);
  CU_ASSERT(xml_business_group->root_node != NULL);
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
  pSuite = CU_add_suite("AgsXmlBusinessGroupTest", ags_xml_business_group_test_init_suite, ags_xml_business_group_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsXmlBusinessGroup get group UUID", ags_xml_business_group_test_get_group_uuid) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlBusinessGroup set group name", ags_xml_business_group_test_set_group_name) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlBusinessGroup get group name", ags_xml_business_group_test_get_group_name) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlBusinessGroup set user", ags_xml_business_group_test_set_user) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlBusinessGroup get user", ags_xml_business_group_test_get_user) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXmlBusinessGroup open filename", ags_xml_business_group_test_open_filename) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
