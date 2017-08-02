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

int ags_task_thread_test_init_suite();
int ags_task_thread_test_clean_suite();

void ags_task_thread_test_append_task();
void ags_task_thread_test_append_tasks();
void ags_task_thread_test_append_cyclic_task();
void ags_task_thread_test_remove_cyclic_task();
void ags_task_thread_test_clear_cache();

void ags_task_thread_test_launch_callback(AgsTask *task,
					  gpointer data);
void ags_task_thread_test_stub_clear_cache(AgsTask *task);

#define AGS_TASK_THREAD_TEST_INITIAL_DELAY (500000)
#define AGS_TASK_THREAD_TEST_DELAY (10000000)

guint launch_counter = 0;
gboolean stub_clear_cache = FALSE;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_task_thread_test_init_suite()
{  
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_task_thread_test_clean_suite()
{
  return(0);
}

void
ags_task_thread_test_append_task()
{
  AgsGenericMainLoop *main_loop;
  AgsTaskThread *task_thread;
  AgsThreadPool *thread_pool;
  AgsTask *task;

  AgsApplicationContext *application_context;

  application_context = ags_thread_application_context_new();

  main_loop = application_context->main_loop;
  
  ags_thread_start(main_loop);

  task_thread = ags_task_thread_new();
  ags_main_loop_set_async_queue(AGS_MAIN_LOOP(main_loop),
				task_thread);
  ags_thread_add_child_extended(main_loop,
				task_thread,
				TRUE, TRUE);

  ags_thread_start(task_thread);
  
  usleep(AGS_TASK_THREAD_TEST_INITIAL_DELAY);

  thread_pool = AGS_TASK_THREAD(task_thread)->thread_pool;
  ags_thread_pool_start(thread_pool);

  usleep(AGS_TASK_THREAD_TEST_INITIAL_DELAY);
  
  /* create some tasks */
  launch_counter = 0;

  task = g_object_new(AGS_TYPE_TASK,
		      NULL);
  g_signal_connect(task, "launch",
		   G_CALLBACK(ags_task_thread_test_launch_callback), NULL);

  ags_task_thread_append_task(task_thread,
			      task);

  task = g_object_new(AGS_TYPE_TASK,
		      NULL);
  g_signal_connect(task, "launch",
		   G_CALLBACK(ags_task_thread_test_launch_callback), NULL);

  ags_task_thread_append_task(task_thread,
			      task);

  task = g_object_new(AGS_TYPE_TASK,
		      NULL);
  g_signal_connect(task, "launch",
		   G_CALLBACK(ags_task_thread_test_launch_callback), NULL);

  ags_task_thread_append_task(task_thread,
			      task);

  /* assert */
  usleep(AGS_TASK_THREAD_TEST_DELAY);

  CU_ASSERT(launch_counter == 3);

  /* clean-up */
  g_object_run_dispose(application_context);
  g_object_unref(application_context);
}

void
ags_task_thread_test_append_tasks()
{
  AgsGenericMainLoop *main_loop;
  AgsTaskThread *task_thread;
  AgsThreadPool *thread_pool;
  AgsTask *task;

  AgsApplicationContext *application_context;

  GList *list0, *list1;  

  application_context = ags_thread_application_context_new();
  
  main_loop = application_context->main_loop;
  
  ags_thread_start(main_loop);

  task_thread = ags_task_thread_new();
  ags_main_loop_set_async_queue(AGS_MAIN_LOOP(main_loop),
				task_thread);
  ags_thread_add_child_extended(main_loop,
				task_thread,
				TRUE, TRUE);

  ags_thread_start(task_thread);
  
  usleep(AGS_TASK_THREAD_TEST_INITIAL_DELAY);

  thread_pool = AGS_TASK_THREAD(task_thread)->thread_pool;
  ags_thread_pool_start(thread_pool);

  usleep(AGS_TASK_THREAD_TEST_INITIAL_DELAY);
  
  /* create some tasks */
  list0 = NULL;
  launch_counter = 0;

  task = g_object_new(AGS_TYPE_TASK,
		      NULL);
  list0 = g_list_prepend(list0,
			 task);  
  g_signal_connect(task, "launch",
		   G_CALLBACK(ags_task_thread_test_launch_callback), NULL);
  
  task = g_object_new(AGS_TYPE_TASK,
		      NULL);
  list0 = g_list_prepend(list0,
			 task);
  g_signal_connect(task, "launch",
		   G_CALLBACK(ags_task_thread_test_launch_callback), NULL);

  ags_task_thread_append_tasks(task_thread,
			       list0);

  list1 = NULL;

  task = g_object_new(AGS_TYPE_TASK,
		      NULL);
  list1 = g_list_prepend(list1,
			 task);
  g_signal_connect(task, "launch",
		   G_CALLBACK(ags_task_thread_test_launch_callback), NULL);

  ags_task_thread_append_tasks(task_thread,
			       list1);

  /* assert */
  usleep(AGS_TASK_THREAD_TEST_DELAY);

  CU_ASSERT(launch_counter == 3);

  /* clean-up */
  g_object_run_dispose(application_context);
  g_object_unref(application_context);
}

void
ags_task_thread_test_append_cyclic_task()
{
  AgsGenericMainLoop *main_loop;
  AgsTaskThread *task_thread;
  AgsThreadPool *thread_pool;
  AgsTask *task;

  AgsApplicationContext *application_context;

  application_context = ags_thread_application_context_new();
  
  main_loop = ags_generic_main_loop_new(application_context);
  g_object_set(application_context,
	       "main-loop", main_loop,
	       NULL);
  
  ags_thread_start(main_loop);

  task_thread = ags_task_thread_new();
  ags_main_loop_set_async_queue(AGS_MAIN_LOOP(main_loop),
				task_thread);
  ags_thread_add_child_extended(main_loop,
				task_thread,
				TRUE, TRUE);

  ags_thread_start(task_thread);
  
  usleep(AGS_TASK_THREAD_TEST_INITIAL_DELAY);

  thread_pool = AGS_TASK_THREAD(task_thread)->thread_pool;
  ags_thread_pool_start(thread_pool);

  usleep(AGS_TASK_THREAD_TEST_INITIAL_DELAY);

  /* create a cyclic tasks */
  launch_counter = 0;

  task = g_object_new(AGS_TYPE_TASK,
		      NULL);
  g_signal_connect(task, "launch",
		   G_CALLBACK(ags_task_thread_test_launch_callback), NULL);

  ags_task_thread_append_cyclic_task(task_thread,
				     task);

  /* assert */
  usleep(AGS_TASK_THREAD_TEST_DELAY);

  CU_ASSERT(launch_counter > 0);

  /* clean-up */
  g_object_run_dispose(application_context);
  g_object_unref(application_context);
}

void
ags_task_thread_test_remove_cyclic_task()
{
  //TODO:JK: implement me, note this is quiet tricky since we are outside of the synced tree
}

void
ags_task_thread_test_clear_cache()
{
  AgsTaskThread *task_thread;
  gpointer ptr;
  
  task_thread = ags_task_thread_new();

  /* stub */
  ptr = AGS_TASK_THREAD_GET_CLASS(task_thread)->clear_cache;
  AGS_TASK_THREAD_GET_CLASS(task_thread)->clear_cache = ags_task_thread_test_stub_clear_cache;

  /* assert */
  ags_task_thread_clear_cache(task_thread);

  CU_ASSERT(stub_clear_cache == TRUE);

  /* reset */
  AGS_TASK_THREAD_GET_CLASS(task_thread)->clear_cache = ptr;
}

void
ags_task_thread_test_launch_callback(AgsTask *task,
				     gpointer data)
{  
  launch_counter++;
}

void
ags_task_thread_test_stub_clear_cache(AgsTask *task)
{
  stub_clear_cache = TRUE;
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
  pSuite = CU_add_suite("AgsTaskThreadTest\0", ags_task_thread_test_init_suite, ags_task_thread_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsTaskThread append task\0", ags_task_thread_test_append_task) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTaskThread append tasks\0", ags_task_thread_test_append_tasks) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTaskThread append cyclic task\0", ags_task_thread_test_append_cyclic_task) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTaskThread remove cyclic task\0", ags_task_thread_test_remove_cyclic_task) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTaskThread clear cache\0", ags_task_thread_test_clear_cache) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
