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

#include <ags/lib/ags_log.h>

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <math.h>

int ags_log_test_init_suite();
int ags_log_test_clean_suite();

void ags_log_test_add_message();
void ags_log_test_get_messages();

#define AGS_LOG_TEST_ADD_MESSAGE_START_ENGINE "start engine"
#define AGS_LOG_TEST_ADD_MESSAGE_LOAD_PLUGINS "load plugins"
#define AGS_LOG_TEST_ADD_MESSAGE_APPLICATION_READY "application ready"
#define AGS_LOG_TEST_ADD_MESSAGE_STOP_ENGINE "stop engine"

#define AGS_LOG_TEST_GET_MESSAGES_DEFAULT "default"

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_log_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_log_test_clean_suite()
{
  return(0);
}

void
ags_log_test_add_message()
{
  AgsLog *log;

  log = ags_log_get_instance();

  /* message - start engine */
  ags_log_add_message(log,
		      AGS_LOG_TEST_ADD_MESSAGE_START_ENGINE);  

  CU_ASSERT(g_list_find_custom(g_atomic_pointer_get(&(log->messages)),
			       AGS_LOG_TEST_ADD_MESSAGE_START_ENGINE,
			       g_strcmp0) != NULL);
  
  /* message - load plugins */
  ags_log_add_message(log,
		      AGS_LOG_TEST_ADD_MESSAGE_LOAD_PLUGINS);  

  CU_ASSERT(g_list_find_custom(g_atomic_pointer_get(&(log->messages)),
			       AGS_LOG_TEST_ADD_MESSAGE_LOAD_PLUGINS,
			       g_strcmp0) != NULL);

  /* message - application ready */
  ags_log_add_message(log,
		      AGS_LOG_TEST_ADD_MESSAGE_APPLICATION_READY);  

  CU_ASSERT(g_list_find_custom(g_atomic_pointer_get(&(log->messages)),
			       AGS_LOG_TEST_ADD_MESSAGE_APPLICATION_READY,
			       g_strcmp0) != NULL);

  /* message - stop engine */
  ags_log_add_message(log,
		      AGS_LOG_TEST_ADD_MESSAGE_STOP_ENGINE);

  CU_ASSERT(g_list_find_custom(g_atomic_pointer_get(&(log->messages)),
			       AGS_LOG_TEST_ADD_MESSAGE_STOP_ENGINE,
			       g_strcmp0) != NULL);

  /* unref log */
  g_object_unref(log);
}

void
ags_log_test_get_messages()
{
  AgsLog *log;

  gchar *str;
  
  guint i;

  log = ags_log_get_instance();

  /* assert initial NULL */
  CU_ASSERT(ags_log_get_messages(log) == NULL);

  /* add messages - #0 */
  i  = 0;

  str = g_strdup_printf("%s",
			AGS_LOG_TEST_GET_MESSAGES_DEFAULT,
			i);
  i++;

  ags_log_add_message(log,
		      str);

  /* add messages - #1 */
  str = g_strdup_printf("%s",
			AGS_LOG_TEST_GET_MESSAGES_DEFAULT,
			i);
  i++;

  ags_log_add_message(log,
		      str);

  /* add messages - #2 */
  i  = 0;

  str = g_strdup_printf("%s",
			AGS_LOG_TEST_GET_MESSAGES_DEFAULT,
			i);
  i++;

  ags_log_add_message(log,
		      str);

  /* assert list length equals 3 */
  CU_ASSERT(g_list_length(ags_log_get_messages(log)) == 3);
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
  pSuite = CU_add_suite("AgsLogTest\0", ags_log_test_init_suite, ags_log_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsLog add message\0", ags_log_test_add_message) == NULL) ||
     (CU_add_test(pSuite, "test of AgsLog get messages\0", ags_log_test_get_messages) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
