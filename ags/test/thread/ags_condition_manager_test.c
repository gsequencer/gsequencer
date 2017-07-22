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

int ags_condition_manager_test_init_suite();
int ags_condition_manager_test_clean_suite();

void ags_condition_manager_test_insert();
void ags_condition_manager_test_remove();
void ags_condition_manager_test_lookup();

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_condition_manager_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_condition_manager_test_clean_suite()
{
  return(0);
}

void
ags_condition_manager_test_insert()
{
  AgsConditionManager *condition_manager;
  AgsThread *thread;

  pthread_cond_t *cond;
  
  condition_manager = ags_condition_manager_get_instance();

  /* create thread and insert */
  thread = g_object_new(AGS_TYPE_THREAD,
			NULL);
  
  cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(cond, NULL);

  ags_condition_manager_insert(condition_manager,
			       thread, cond);

  /* assert contains */
  CU_ASSERT(g_hash_table_contains(condition_manager->lock_object, thread) == TRUE);

  /* create thread and insert */
  thread = g_object_new(AGS_TYPE_THREAD,
			NULL);
  
  cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(cond, NULL);

  ags_condition_manager_insert(condition_manager,
			       thread, cond);

  /* assert contains */
  CU_ASSERT(g_hash_table_contains(condition_manager->lock_object, thread) == TRUE);

  g_object_unref(condition_manager);
}

void
ags_condition_manager_test_remove()
{
  AgsConditionManager *condition_manager;
  AgsThread *thread[3];

  pthread_cond_t *cond;
  
  condition_manager = ags_condition_manager_get_instance();

  /* create thread and insert */
  thread[0] = g_object_new(AGS_TYPE_THREAD,
			   NULL);
  
  cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(cond, NULL);

  ags_condition_manager_insert(condition_manager,
			       thread[0], cond);

  /* create thread and insert */
  thread[1] = g_object_new(AGS_TYPE_THREAD,
			NULL);
  
  cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(cond, NULL);

  ags_condition_manager_insert(condition_manager,
			       thread[1], cond);

  /* create thread and insert */
  thread[2] = g_object_new(AGS_TYPE_THREAD,
			NULL);
  
  cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(cond, NULL);

  ags_condition_manager_insert(condition_manager,
			       thread[2], cond);

  /* assert contains */
  ags_condition_manager_remove(condition_manager,
			       thread[1]);
  CU_ASSERT(g_hash_table_contains(condition_manager->lock_object, thread[1]) == FALSE);

  ags_condition_manager_remove(condition_manager,
			       thread[2]);
  CU_ASSERT(g_hash_table_contains(condition_manager->lock_object, thread[2]) == FALSE);

  ags_condition_manager_remove(condition_manager,
			       thread[0]);
  CU_ASSERT(g_hash_table_contains(condition_manager->lock_object, thread[0]) == FALSE);

  g_object_unref(condition_manager);
}

void
ags_condition_manager_test_lookup()
{
  AgsConditionManager *condition_manager;
  AgsThread *thread[2];

  pthread_cond_t *cond[2];
  
  condition_manager = ags_condition_manager_get_instance();

  /* create thread and insert */
  thread[0] = g_object_new(AGS_TYPE_THREAD,
			   NULL);
  
  cond[0] = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(cond[0], NULL);

  ags_condition_manager_insert(condition_manager,
			       thread[0], cond[0]);

  /* assert contains */
  CU_ASSERT(g_hash_table_contains(condition_manager->lock_object, thread) == TRUE);

  /* create thread and insert */
  thread[1] = g_object_new(AGS_TYPE_THREAD,
			   NULL);
  
  cond[1] = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(cond[1], NULL);

  ags_condition_manager_insert(condition_manager,
			       thread[1], cond[1]);

  /* assert lookup */
  CU_ASSERT(ags_condition_manager_lookup(condition_manager, thread[0]) == cond[0]);
  CU_ASSERT(ags_condition_manager_lookup(condition_manager, thread[1]) == cond[1]);

  g_object_unref(condition_manager);
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
  pSuite = CU_add_suite("AgsConditionManagerTest\0", ags_condition_manager_test_init_suite, ags_condition_manager_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsConditionManager insert\0", ags_condition_manager_test_insert) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConditionManager remove\0", ags_condition_manager_test_remove) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConditionManager lookup\0", ags_condition_manager_test_lookup) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

