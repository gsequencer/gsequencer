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
#include <ags/libags-audio.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_lv2_worker_manager_test_init_suite();
int ags_lv2_worker_manager_test_clean_suite();

void ags_lv2_worker_manager_test_pull_worker();

#define AGS_LV2_WORKER_MANAGER_TEST_PULL_WORKER_DELAY (5000000)

/* Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_worker_manager_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_lv2_worker_manager_test_clean_suite()
{
  return(0);
}

void
ags_lv2_worker_manager_test_pull_worker()
{
  AgsLv2WorkerManager *lv2_worker_manager;
  AgsThreadPool *thread_pool;
  
  GObject *worker;
  
  lv2_worker_manager = ags_lv2_worker_manager_get_instance();

  thread_pool = g_object_new(AGS_TYPE_THREAD_POOL,
			     NULL);
  lv2_worker_manager->thread_pool = thread_pool;

  ags_thread_pool_start(thread_pool);

  usleep(AGS_LV2_WORKER_MANAGER_TEST_PULL_WORKER_DELAY);
  
  worker = ags_lv2_worker_manager_pull_worker(lv2_worker_manager);

  CU_ASSERT(AGS_IS_LV2_WORKER(worker) == TRUE);

  g_object_unref(lv2_worker_manager);
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
  pSuite = CU_add_suite("AgsLv2WorkerManagerTest", ags_lv2_worker_manager_test_init_suite, ags_lv2_worker_manager_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsLv2WorkerManager pull worker", ags_lv2_worker_manager_test_pull_worker) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
