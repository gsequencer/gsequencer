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

int ags_concurrency_provider_test_init_suite();
int ags_concurrency_provider_test_clean_suite();

void ags_concurrency_provider_test_get_main_loop();
void ags_concurrency_provider_test_set_main_loop();
void ags_concurrency_provider_test_get_task_launcher();
void ags_concurrency_provider_test_set_task_launcher();
void ags_concurrency_provider_test_get_thread_pool();
void ags_concurrency_provider_test_set_thread_pool();
void ags_concurrency_provider_test_get_worker();
void ags_concurrency_provider_test_set_worker();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_concurrency_provider_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_concurrency_provider_test_clean_suite()
{
  return(0);
}

void
ags_concurrency_provider_test_get_main_loop()
{
  AgsThreadApplicationContext *thread_application_context;

  thread_application_context = ags_thread_application_context_new();
  
  CU_ASSERT(ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(thread_application_context)) == NULL);

  AGS_APPLICATION_CONTEXT(thread_application_context)->main_loop = ags_generic_main_loop_new();
  
  CU_ASSERT(ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(thread_application_context)) == AGS_APPLICATION_CONTEXT(thread_application_context)->main_loop);
}

void
ags_concurrency_provider_test_set_main_loop()
{
  AgsThreadApplicationContext *thread_application_context;
  AgsThread *main_loop;
  
  thread_application_context = ags_thread_application_context_new();
  
  main_loop = ags_generic_main_loop_new();

  ags_concurrency_provider_set_main_loop(AGS_CONCURRENCY_PROVIDER(thread_application_context), main_loop);

  CU_ASSERT(AGS_APPLICATION_CONTEXT(thread_application_context)->main_loop == main_loop);
}

void
ags_concurrency_provider_test_get_task_launcher()
{
  AgsThreadApplicationContext *thread_application_context;

  thread_application_context = ags_thread_application_context_new();
  
  CU_ASSERT(ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(thread_application_context)) == NULL);

  AGS_APPLICATION_CONTEXT(thread_application_context)->task_launcher = ags_task_launcher_new();
  
  CU_ASSERT(ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(thread_application_context)) == AGS_APPLICATION_CONTEXT(thread_application_context)->task_launcher);
}

void
ags_concurrency_provider_test_set_task_launcher()
{
  AgsThreadApplicationContext *thread_application_context;
  AgsTaskLauncher *task_launcher;
  
  thread_application_context = ags_thread_application_context_new();
  
  task_launcher = ags_task_launcher_new();

  ags_concurrency_provider_set_task_launcher(AGS_CONCURRENCY_PROVIDER(thread_application_context), task_launcher);

  CU_ASSERT(AGS_APPLICATION_CONTEXT(thread_application_context)->task_launcher == task_launcher);
}

void
ags_concurrency_provider_test_get_thread_pool()
{
  AgsThreadApplicationContext *thread_application_context;

  thread_application_context = ags_thread_application_context_new();
  
  CU_ASSERT(ags_concurrency_provider_get_thread_pool(AGS_CONCURRENCY_PROVIDER(thread_application_context)) == NULL);

  thread_application_context->thread_pool = ags_thread_pool_new(NULL);
  
  CU_ASSERT(ags_concurrency_provider_get_thread_pool(AGS_CONCURRENCY_PROVIDER(thread_application_context)) == thread_application_context->thread_pool);
}

void
ags_concurrency_provider_test_set_thread_pool()
{
  AgsThreadApplicationContext *thread_application_context;
  AgsThreadPool *thread_pool;
  
  thread_application_context = ags_thread_application_context_new();
  
  thread_pool = ags_thread_pool_new(NULL);

  ags_concurrency_provider_set_thread_pool(AGS_CONCURRENCY_PROVIDER(thread_application_context), thread_pool);

  CU_ASSERT(thread_application_context->thread_pool == thread_pool);
}

void
ags_concurrency_provider_test_get_worker()
{
  AgsThreadApplicationContext *thread_application_context;

  GList *start_worker, *worker;
  GList *copy_start_worker, *copy_worker;

  guint i;
  
  thread_application_context = ags_thread_application_context_new();

  start_worker = NULL;
  
  start_worker = g_list_prepend(start_worker,
				ags_worker_thread_new());
  start_worker = g_list_prepend(start_worker,
				ags_worker_thread_new());
  start_worker = g_list_prepend(start_worker,
				ags_worker_thread_new());
  
  CU_ASSERT(ags_concurrency_provider_get_worker(AGS_CONCURRENCY_PROVIDER(thread_application_context)) == NULL);

  thread_application_context->worker = start_worker;

  copy_start_worker = ags_concurrency_provider_get_worker(AGS_CONCURRENCY_PROVIDER(thread_application_context));

  worker = start_worker;
  copy_worker = copy_start_worker;

  for(i = 0; worker != NULL && copy_worker != NULL && worker->data == copy_worker->data; i++){
    worker = worker->next;
    copy_worker = copy_worker->next;
  }
  
  CU_ASSERT(i == 3);
}

void
ags_concurrency_provider_test_set_worker()
{
  AgsThreadApplicationContext *thread_application_context;
  GList *start_worker;

  thread_application_context = ags_thread_application_context_new();

  start_worker = NULL;
  
  start_worker = g_list_prepend(start_worker,
				ags_worker_thread_new());
  start_worker = g_list_prepend(start_worker,
				ags_worker_thread_new());
  start_worker = g_list_prepend(start_worker,
				ags_worker_thread_new());

  ags_concurrency_provider_set_worker(AGS_CONCURRENCY_PROVIDER(thread_application_context), start_worker);

  CU_ASSERT(thread_application_context->worker == start_worker);
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
  pSuite = CU_add_suite("AgsConcurrencyProviderTest", ags_concurrency_provider_test_init_suite, ags_concurrency_provider_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsConcurrencyProvider get main loop", ags_concurrency_provider_test_get_main_loop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConcurrencyProvider set main loop", ags_concurrency_provider_test_set_main_loop) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConcurrencyProvider get task launcher", ags_concurrency_provider_test_get_task_launcher) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConcurrencyProvider set task launcher", ags_concurrency_provider_test_set_task_launcher) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConcurrencyProvider get thread pool", ags_concurrency_provider_test_get_thread_pool) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConcurrencyProvider set thread pool", ags_concurrency_provider_test_set_thread_pool) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConcurrencyProvider get worker", ags_concurrency_provider_test_get_worker) == NULL) ||
     (CU_add_test(pSuite, "test of AgsConcurrencyProvider set worker", ags_concurrency_provider_test_set_worker) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

