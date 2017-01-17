/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015,2017 Joël Krähemann
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

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_main_loop.h>
#include <ags/object/ags_async_queue.h>

#include <ags/thread/ags_thread-posix.h>
#include <ags/thread/ags_generic_main_loop.h>
#include <ags/thread/ags_task_thread.h>

int ags_functional_thread_test_init_suite();
int ags_functional_thread_test_clean_suite();

void ags_functional_thread_test_synchronization();

AgsApplicationContext *application_context;

volatile struct timespec *synchronization_idle;

typedef enum{
  AGS_FUNCTIONAL_THREAD_TEST_SYNCHRONIZATION_STRATEGY_NANOSLEEP       = 1,
  AGS_FUNCTIONAL_THREAD_TEST_SYNCHRONIZATION_STRATEGY_COMPUTE_ADD     = 1 <<  1,
  AGS_FUNCTIONAL_THREAD_TEST_SYNCHRONIZATION_STRATEGY_COMPUTE_SIN     = 1 <<  2,
  AGS_FUNCTIONAL_THREAD_TEST_SYNCHRONIZATION_STRATEGY_COMPUTE_LOG     = 1 <<  3,
  AGS_FUNCTIONAL_THREAD_TEST_SYNCHRONIZATION_STRATEGY_HARD_LIMIT      = 1 <<  4,
  AGS_FUNCTIONAL_THREAD_TEST_SYNCHRONIZATION_STRATEGY_SOFT_LIMIT      = 1 <<  5,
  AGS_FUNCTIONAL_THREAD_TEST_SYNCHRONIZATION_STRATEGY_INTERRUPT       = 1 <<  6,
  AGS_FUNCTIONAL_THREAD_TEST_SYNCHRONIZATION_STRATEGY_START_QUEUE     = 1 <<  7,
  AGS_FUNCTIONAL_THREAD_TEST_SYNCHRONIZATION_STRATEGY_START_WAIT      = 1 <<  8,
  AGS_FUNCTIONAL_THREAD_TEST_SYNCHRONIZATION_STRATEGY_START_CONDITION = 1 <<  9,
}AgsFunctionalThreadTestSynchronizationStrategyFlags;

struct _AgsFunctionalThreadTestSynchronizationStrategy{
  guint strategy_flags;
  
  gchar *strategy_name;

  guint buffer_length;
  GType buffer_type;
  
  void *buffer;
  
  void (*compute_idle)(struct timespec *idle_source);
};

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_thread_test_init_suite()
{
  struct timespec *idle;

  /* instantiate application context */
  application_context = ags_application_context_new(NULL,
						    NULL);

  /* allocate and set idle */
  idle = (struct timespec *) malloc(sizeof(struct timespec));

  idle->tv_sec = 0;
  idle->tv_nsec = 0;
  
  g_atomic_pointer_set(&(synchronization_idle),
		       idle);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_thread_test_clean_suite()
{
  free(g_atomic_pointer_get(&(synchronization_idle)));
  
  return(0);
}

void
ags_functional_thread_test_synchronization()
{
  //TODO:JK: implement me
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
  pSuite = CU_add_suite("AgsFunctionalThreadTest\0", ags_functional_thread_test_init_suite, ags_functional_thread_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsThread doing synchronization\0", ags_functional_thread_test_synchronization) == NULL)){
      CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

