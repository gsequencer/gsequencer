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

int ags_mutex_manager_test_init_suite();
int ags_mutex_manager_test_clean_suite();

void ags_mutex_manager_test_insert();
void ags_mutex_manager_test_remove();
void ags_mutex_manager_test_lookup();

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_mutex_manager_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_mutex_manager_test_clean_suite()
{
  return(0);
}

void
ags_mutex_manager_test_insert()
{
  AgsMutexManager *mutex_manager;
  AgsThread *thread;

  pthread_mutex_t *mutex;
  
  mutex_manager = ags_mutex_manager_get_instance();

  /* create thread and insert */
  thread = g_object_new(AGS_TYPE_THREAD,
			NULL);
  
  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex, NULL);

  ags_mutex_manager_insert(mutex_manager,
			   thread, mutex);

  /* assert contains */
  CU_ASSERT(g_hash_table_contains(mutex_manager->lock_object, thread) == TRUE);

  /* create thread and insert */
  thread = g_object_new(AGS_TYPE_THREAD,
			NULL);
  
  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex, NULL);

  ags_mutex_manager_insert(mutex_manager,
			   thread, mutex);

  /* assert contains */
  CU_ASSERT(g_hash_table_contains(mutex_manager->lock_object, thread) == TRUE);

  g_object_unref(mutex_manager);
}

void
ags_mutex_manager_test_remove()
{
  AgsMutexManager *mutex_manager;
  AgsThread *thread[3];

  pthread_mutex_t *mutex;
  
  mutex_manager = ags_mutex_manager_get_instance();

  /* create thread and insert */
  thread[0] = g_object_new(AGS_TYPE_THREAD,
			   NULL);
  
  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex, NULL);

  ags_mutex_manager_insert(mutex_manager,
			   thread[0], mutex);

  /* create thread and insert */
  thread[1] = g_object_new(AGS_TYPE_THREAD,
			   NULL);
  
  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex, NULL);

  ags_mutex_manager_insert(mutex_manager,
			   thread[1], mutex);

  /* create thread and insert */
  thread[2] = g_object_new(AGS_TYPE_THREAD,
			   NULL);
  
  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex, NULL);

  ags_mutex_manager_insert(mutex_manager,
			   thread[2], mutex);

  /* assert contains */
  ags_mutex_manager_remove(mutex_manager,
			   thread[1]);
  CU_ASSERT(g_hash_table_contains(mutex_manager->lock_object, thread[1]) == FALSE);

  ags_mutex_manager_remove(mutex_manager,
			   thread[2]);
  CU_ASSERT(g_hash_table_contains(mutex_manager->lock_object, thread[2]) == FALSE);

  ags_mutex_manager_remove(mutex_manager,
			   thread[0]);
  CU_ASSERT(g_hash_table_contains(mutex_manager->lock_object, thread[0]) == FALSE);

  g_object_unref(mutex_manager);
}

void
ags_mutex_manager_test_lookup()
{
  AgsMutexManager *mutex_manager;
  AgsThread *thread[2];

  pthread_mutex_t *mutex[2];
  
  mutex_manager = ags_mutex_manager_get_instance();

  /* create thread and insert */
  thread[0] = g_object_new(AGS_TYPE_THREAD,
			   NULL);
  
  mutex[0] = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex[0], NULL);

  ags_mutex_manager_insert(mutex_manager,
			   thread[0], mutex[0]);

  /* create thread and insert */
  thread[1] = g_object_new(AGS_TYPE_THREAD,
			   NULL);
  
  mutex[1] = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex[1], NULL);

  ags_mutex_manager_insert(mutex_manager,
			   thread[1], mutex[1]);

  /* assert lookup */
  CU_ASSERT(ags_mutex_manager_lookup(mutex_manager, thread[0]) == mutex[0]);
  CU_ASSERT(ags_mutex_manager_lookup(mutex_manager, thread[1]) == mutex[1]);

  g_object_unref(mutex_manager);
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
  pSuite = CU_add_suite("AgsMutexManagerTest\0", ags_mutex_manager_test_init_suite, ags_mutex_manager_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsMutexManager insert\0", ags_mutex_manager_test_insert) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMutexManager remove\0", ags_mutex_manager_test_remove) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMutexManager lookup\0", ags_mutex_manager_test_lookup) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
