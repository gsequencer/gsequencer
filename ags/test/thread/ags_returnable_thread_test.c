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

int ags_returnable_thread_test_init_suite();
int ags_returnable_thread_test_clean_suite();

void ags_returnable_thread_test_safe_run();
void ags_returnable_thread_test_connect_safe_run();
void ags_returnable_thread_test_disconnect_safe_run();

void ags_returnable_thread_test_stub_safe_run(AgsReturnableThread *returnable_thread);

void ags_returnable_thread_test_safe_run_callback(AgsReturnableThread *returnable_thread,
						  gpointer data);

gboolean stub_safe_run = FALSE;
gboolean safe_run_callback;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_returnable_thread_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_returnable_thread_test_clean_suite()
{
  return(0);
}

void
ags_returnable_thread_test_safe_run()
{
  AgsReturnableThread *returnable_thread;

  gpointer ptr;
  
  returnable_thread = g_object_new(AGS_TYPE_RETURNABLE_THREAD,
				   NULL);

  /* stub safe-run */
  ptr = AGS_RETURNABLE_THREAD_GET_CLASS(returnable_thread)->safe_run;
  AGS_RETURNABLE_THREAD_GET_CLASS(returnable_thread)->safe_run = ags_returnable_thread_test_stub_safe_run;

  /* assert safe-run */
  ags_returnable_thread_safe_run(returnable_thread);

  CU_ASSERT(stub_safe_run == TRUE);
  
  AGS_RETURNABLE_THREAD_GET_CLASS(returnable_thread)->safe_run = ptr;
}

void
ags_returnable_thread_test_connect_safe_run()
{
  AgsReturnableThread *returnable_thread;

  returnable_thread = g_object_new(AGS_TYPE_RETURNABLE_THREAD,
				   NULL);

  /* connect safe run and assert */
  ags_returnable_thread_connect_safe_run(returnable_thread,
					 ags_returnable_thread_test_safe_run_callback);

  CU_ASSERT(returnable_thread->handler > 0);
  
  /* safe run and assert */
  safe_run_callback = FALSE;

  ags_returnable_thread_safe_run(returnable_thread);

  CU_ASSERT(safe_run_callback == TRUE);
}

void
ags_returnable_thread_test_disconnect_safe_run()
{
  AgsReturnableThread *returnable_thread;

  returnable_thread = g_object_new(AGS_TYPE_RETURNABLE_THREAD,
				   NULL);
  ags_returnable_thread_connect_safe_run(returnable_thread,
					 ags_returnable_thread_test_safe_run_callback);

  /* connect safe run and assert */
  ags_returnable_thread_disconnect_safe_run(returnable_thread);

  CU_ASSERT(returnable_thread->handler == 0);
  
  /* safe run and assert */
  safe_run_callback = FALSE;

  ags_returnable_thread_safe_run(returnable_thread);

  CU_ASSERT(safe_run_callback == FALSE);
}

void
ags_returnable_thread_test_stub_safe_run(AgsReturnableThread *returnable_thread)
{
  stub_safe_run = TRUE;
}

void
ags_returnable_thread_test_safe_run_callback(AgsReturnableThread *returnable_thread,
					     gpointer data)
{
  safe_run_callback = TRUE;
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
  pSuite = CU_add_suite("AgsReturnableThreadTest\0", ags_returnable_thread_test_init_suite, ags_returnable_thread_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsReturnableThread safe run\0", ags_returnable_thread_test_safe_run) == NULL) ||
     (CU_add_test(pSuite, "test of AgsReturnableThread connect safe run\0", ags_returnable_thread_test_connect_safe_run) == NULL) ||
     (CU_add_test(pSuite, "test of AgsReturnableThread disconnect safe run\0", ags_returnable_thread_test_disconnect_safe_run) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
