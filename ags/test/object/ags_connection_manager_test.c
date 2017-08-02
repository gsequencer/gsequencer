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

int ags_connection_manager_test_init_suite();
int ags_connection_manager_test_clean_suite();

void ags_connection_manager_test_get_connection();
void ags_connection_manager_test_add_connection();
void ags_connection_manager_test_remove_connection();

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_connection_manager_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_connection_manager_test_clean_suite()
{
  return(0);
}

void
ags_connection_manager_test_get_connection()
{
  AgsConnectionManager *connection_manager;
  AgsConnection *connection;

  GList *list;
  
  connection_manager = ags_connection_manager_get_instance();

  /* create some connection */
  list = NULL;
  
  connection = g_object_new(AGS_TYPE_CONNECTION,
			    NULL);
  list = g_list_prepend(list,
			connection);
  
  connection = g_object_new(AGS_TYPE_CONNECTION,
			    NULL);
  list = g_list_prepend(list,
			connection);

  connection = g_object_new(AGS_TYPE_CONNECTION,
			    NULL);
  list = g_list_prepend(list,
			connection);

  connection_manager->connection = list;

  /* assert get connection */
  CU_ASSERT(g_list_length(ags_connection_manager_get_connection(connection_manager)) == 3);  

  g_object_unref(connection_manager);
}

void
ags_connection_manager_test_add_connection()
{
  AgsConnectionManager *connection_manager;
  AgsConnection *connection;
  
  connection_manager = ags_connection_manager_get_instance();

  /* add connection */
  connection = g_object_new(AGS_TYPE_CONNECTION,
			    NULL);
  ags_connection_manager_add_connection(connection_manager,
					connection);
  CU_ASSERT(g_list_length(connection_manager->connection) == 1);  

  /* add connection */
  connection = g_object_new(AGS_TYPE_CONNECTION,
			    NULL);
  ags_connection_manager_add_connection(connection_manager,
					connection);
  CU_ASSERT(g_list_length(connection_manager->connection) == 2);  

  /* add connection */
  connection = g_object_new(AGS_TYPE_CONNECTION,
			    NULL);
  ags_connection_manager_add_connection(connection_manager,
					connection);
  CU_ASSERT(g_list_length(connection_manager->connection) == 3);

  g_object_unref(connection_manager);
}

void
ags_connection_manager_test_remove_connection()
{
  AgsConnectionManager *connection_manager;
  AgsConnection *connection[3];
  
  connection_manager = ags_connection_manager_get_instance();

  /* add connection */
  connection[0] = g_object_new(AGS_TYPE_CONNECTION,
			       NULL);
  ags_connection_manager_add_connection(connection_manager,
					connection[0]);

  /* add connection */
  connection[1] = g_object_new(AGS_TYPE_CONNECTION,
			       NULL);
  ags_connection_manager_add_connection(connection_manager,
					connection[1]);
  
  /* add connection */
  connection[2] = g_object_new(AGS_TYPE_CONNECTION,
			       NULL);
  ags_connection_manager_add_connection(connection_manager,
					connection[2]);

  /* remove and assert length */
  ags_connection_manager_remove_connection(connection_manager,
					   connection[1]);
  CU_ASSERT(g_list_length(connection_manager->connection) == 2);

  ags_connection_manager_remove_connection(connection_manager,
					   connection[0]);
  CU_ASSERT(g_list_length(connection_manager->connection) == 1);

  ags_connection_manager_remove_connection(connection_manager,
					   connection[2]);
  CU_ASSERT(connection_manager->connection == NULL);

  g_object_unref(connection_manager);
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
  pSuite = CU_add_suite("AgsConnectionManagerTest\0", ags_connection_manager_test_init_suite, ags_connection_manager_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsConnectionManager get connection\0", ags_connection_manager_test_get_connection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConnectionManager add connection\0", ags_connection_manager_test_add_connection) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConnectionManager remove connection\0", ags_connection_manager_test_remove_connection) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
