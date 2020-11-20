/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/libags.h>

int ags_generic_main_loop_test_init_suite();
int ags_generic_main_loop_test_clean_suite();

void ags_generic_main_loop_test_run();

void ags_generic_main_loop_test_thread_run_callback(AgsThread *thread, gpointer user_data);

AgsThreadApplicationContext *thread_application_context;

volatile gint run_count = 0;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_generic_main_loop_test_init_suite()
{
  thread_application_context = ags_thread_application_context_new();
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_generic_main_loop_test_clean_suite()
{
  return(0);
}

void
ags_generic_main_loop_test_thread_run_callback(AgsThread *thread, gpointer user_data)
{
  g_atomic_int_inc(&run_count);
}

void
ags_generic_main_loop_test_run()
{
  AgsGenericMainLoop *generic_main_loop;

  AgsThread *thread;

  gdouble frequency;
  
  generic_main_loop =
    AGS_APPLICATION_CONTEXT(thread_application_context)->main_loop = ags_generic_main_loop_new();
  
  ags_thread_start(generic_main_loop);

  thread = ags_thread_new();

  g_signal_connect(thread, "run",
		   G_CALLBACK(ags_generic_main_loop_test_thread_run_callback), NULL);

  ags_thread_add_child_extended(generic_main_loop,
				thread,
				FALSE, FALSE);

  g_usleep(3 * G_TIME_SPAN_SECOND);

  frequency = 0.0;

  g_object_get(thread,
	       "frequency", &frequency,
	       NULL);
  
  CU_ASSERT(g_atomic_int_get(&run_count) > (gint) frequency);
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
  pSuite = CU_add_suite("AgsConcurrencyProviderTest", ags_generic_main_loop_test_init_suite, ags_generic_main_loop_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsConcurrencyProvider run", ags_generic_main_loop_test_run) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

