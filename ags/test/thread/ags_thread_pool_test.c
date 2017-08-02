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

int ags_thread_pool_test_init_suite();
int ags_thread_pool_test_clean_suite();

void ags_thread_pool_test_pull();
void ags_thread_pool_test_start();

void ags_thread_pool_stub_start(AgsThreadPool *thread_pool);

#define AGS_THREAD_POOL_TEST_INITIAL_DELAY (5000000)

gboolean stub_start = FALSE;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_thread_pool_test_init_suite()
{  
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_thread_pool_test_clean_suite()
{
  return(0);
}

void
ags_thread_pool_test_pull()
{
  AgsThreadPool *thread_pool;
  AgsThread *parent;
  AgsReturnableThread *returnable_thread;

  parent = g_object_new(AGS_TYPE_THREAD,
			NULL);
  
  thread_pool = ags_thread_pool_new(parent);
  ags_thread_pool_start(thread_pool);

  usleep(AGS_THREAD_POOL_TEST_INITIAL_DELAY);
  
  /* pull and assert */
  returnable_thread = ags_thread_pool_pull(thread_pool);
  CU_ASSERT(AGS_IS_RETURNABLE_THREAD(returnable_thread) &&
	    (AGS_THREAD_RUNNING & (g_atomic_int_get(&(AGS_THREAD(returnable_thread)->flags)))) != 0);
  
  returnable_thread = ags_thread_pool_pull(thread_pool);
  CU_ASSERT(AGS_IS_RETURNABLE_THREAD(returnable_thread) &&
	    (AGS_THREAD_RUNNING & (g_atomic_int_get(&(AGS_THREAD(returnable_thread)->flags)))) != 0);

  returnable_thread = ags_thread_pool_pull(thread_pool);
  CU_ASSERT(AGS_IS_RETURNABLE_THREAD(returnable_thread) &&
	    (AGS_THREAD_RUNNING & (g_atomic_int_get(&(AGS_THREAD(returnable_thread)->flags)))) != 0);
}

void
ags_thread_pool_test_start()
{
  AgsThreadPool *thread_pool;

  gpointer ptr;
  
  thread_pool = ags_thread_pool_new(NULL);

  ptr = AGS_THREAD_POOL_GET_CLASS(thread_pool)->start;

  /* stub and assert */
  AGS_THREAD_POOL_GET_CLASS(thread_pool)->start = ags_thread_pool_stub_start;
  
  ags_thread_pool_start(thread_pool);

  CU_ASSERT(stub_start == TRUE);

  /* reset */
  AGS_THREAD_POOL_GET_CLASS(thread_pool)->start = ptr;
}

void
ags_thread_pool_stub_start(AgsThreadPool *thread_pool)
{
  stub_start = TRUE;
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C\0");
  putenv("LANG=C\0");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsThreadPoolTest\0", ags_thread_pool_test_init_suite, ags_thread_pool_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsThreadPool pull\0", ags_thread_pool_test_pull) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThreadPool start\0", ags_thread_pool_test_start) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
