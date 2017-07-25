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

int ags_worker_thread_test_init_suite();
int ags_worker_thread_test_clean_suite();

void ags_worker_thread_test_do_poll();

void ags_worker_thread_test_stub_do_poll(AgsWorkerThread *worker_thread);

gboolean stub_do_poll = FALSE;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_worker_thread_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_worker_thread_test_clean_suite()
{
  return(0);
}

void
ags_worker_thread_test_do_poll()
{
  AgsWorkerThread *worker_thread;
  
  gpointer ptr;
  
  worker_thread = g_object_new(AGS_TYPE_WORKER_THREAD,
			       NULL);
  
  ptr = AGS_WORKER_THREAD_GET_CLASS(worker_thread)->do_poll;
  AGS_WORKER_THREAD_GET_CLASS(worker_thread)->do_poll = ags_worker_thread_test_stub_do_poll;

  /* assert do poll */
  ags_worker_thread_do_poll(worker_thread);

  CU_ASSERT(stub_do_poll == TRUE);
  AGS_WORKER_THREAD_GET_CLASS(worker_thread)->do_poll = ptr;
}

void
ags_worker_thread_test_stub_do_poll(AgsWorkerThread *worker_thread)
{
  stub_do_poll = TRUE;
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
  pSuite = CU_add_suite("AgsWorkerThreadTest\0", ags_worker_thread_test_init_suite, ags_worker_thread_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsWorkerThread do poll\0", ags_worker_thread_test_do_poll) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
