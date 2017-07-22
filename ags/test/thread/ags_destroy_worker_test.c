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

int ags_destroy_worker_test_init_suite();
int ags_destroy_worker_test_clean_suite();

void ags_destroy_worker_test_add();

void ags_destroy_worker_test_stub_finalize(GObject *gobject);

#define AGS_DESTROY_WORKER_TEST_ADD_TOTAL (AGS_DESTROY_WORKER_TEST_ADD_STAGE_0 + \
					   AGS_DESTROY_WORKER_TEST_ADD_STAGE_1 + \
					   AGS_DESTROY_WORKER_TEST_ADD_STAGE_2 + \
					   AGS_DESTROY_WORKER_TEST_ADD_STAGE_3 + \
					   AGS_DESTROY_WORKER_TEST_ADD_STAGE_4)
#define AGS_DESTROY_WORKER_TEST_ADD_INITIAL_DELAY (4000)
#define AGS_DESTROY_WORKER_TEST_ADD_FINAL_DELAY (5000000)
#define AGS_DESTROY_WORKER_TEST_ADD_STAGE_0 (8)
#define AGS_DESTROY_WORKER_TEST_ADD_STAGE_0_DELAY (4000)
#define AGS_DESTROY_WORKER_TEST_ADD_STAGE_1 (3)
#define AGS_DESTROY_WORKER_TEST_ADD_STAGE_1_DELAY (8000)
#define AGS_DESTROY_WORKER_TEST_ADD_STAGE_2 (2)
#define AGS_DESTROY_WORKER_TEST_ADD_STAGE_2_DELAY (1500000)
#define AGS_DESTROY_WORKER_TEST_ADD_STAGE_3 (5)
#define AGS_DESTROY_WORKER_TEST_ADD_STAGE_3_DELAY (500000)
#define AGS_DESTROY_WORKER_TEST_ADD_STAGE_4 (7)  
#define AGS_DESTROY_WORKER_TEST_ADD_STAGE_4_DELAY (125000)

guint n_finalize = 0;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_destroy_worker_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_destroy_worker_test_clean_suite()
{
  return(0);
}

void
ags_destroy_worker_test_add()
{
  AgsDestroyWorker *destroy_worker;
  AgsThread *thread[AGS_DESTROY_WORKER_TEST_ADD_TOTAL];
  
  guint i, nth;
  
  destroy_worker = ags_destroy_worker_new();
  ags_thread_start(destroy_worker);

  for(i = 0; i < AGS_DESTROY_WORKER_TEST_ADD_TOTAL; i++){
    thread[i] = g_object_new(AGS_TYPE_THREAD,
			     NULL);
    G_OBJECT_GET_CLASS(thread[i])->finalize = ags_destroy_worker_test_stub_finalize;
  }

  /*  */
  nth = 0;  

  usleep(AGS_DESTROY_WORKER_TEST_ADD_INITIAL_DELAY);

  for(i = 0; i < AGS_DESTROY_WORKER_TEST_ADD_STAGE_0; i++, nth++){
    ags_destroy_worker_add(destroy_worker,
			   thread[nth], g_object_unref);
  }

  usleep(AGS_DESTROY_WORKER_TEST_ADD_STAGE_0_DELAY);

  for(i = 0; i < AGS_DESTROY_WORKER_TEST_ADD_STAGE_1; i++, nth++){
    ags_destroy_worker_add(destroy_worker,
			   thread[nth], g_object_unref);
  }

  usleep(AGS_DESTROY_WORKER_TEST_ADD_STAGE_1_DELAY);

  for(i = 0; i < AGS_DESTROY_WORKER_TEST_ADD_STAGE_2; i++, nth++){
    ags_destroy_worker_add(destroy_worker,
			   thread[nth], g_object_unref);
  }

  usleep(AGS_DESTROY_WORKER_TEST_ADD_STAGE_2_DELAY);

  for(i = 0; i < AGS_DESTROY_WORKER_TEST_ADD_STAGE_3; i++, nth++){
    ags_destroy_worker_add(destroy_worker,
			   thread[nth], g_object_unref);
  }

  usleep(AGS_DESTROY_WORKER_TEST_ADD_STAGE_3_DELAY);

  for(i = 0; i < AGS_DESTROY_WORKER_TEST_ADD_STAGE_4; i++, nth++){
    ags_destroy_worker_add(destroy_worker,
			   thread[nth], g_object_unref);
  }

  usleep(AGS_DESTROY_WORKER_TEST_ADD_STAGE_4_DELAY);

  /* assert */
  usleep(AGS_DESTROY_WORKER_TEST_ADD_FINAL_DELAY);
    
  CU_ASSERT(n_finalize == AGS_DESTROY_WORKER_TEST_ADD_TOTAL);
}

void
ags_destroy_worker_test_stub_finalize(GObject *gobject)
{
  n_finalize++;
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
  pSuite = CU_add_suite("AgsDestroyWorkerTest\0", ags_destroy_worker_test_init_suite, ags_destroy_worker_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsDestroyWorker add\0", ags_destroy_worker_test_add) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

