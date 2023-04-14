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

int ags_task_launcher_test_init_suite();
int ags_task_launcher_test_clean_suite();

void ags_task_launcher_test_add_task();
void ags_task_launcher_test_add_task_all();
void ags_task_launcher_test_add_cyclic_task();
void ags_task_launcher_test_remove_cyclic_task();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_task_launcher_test_init_suite()
{  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_task_launcher_test_clean_suite()
{
  return(0);
}

void
ags_task_launcher_test_add_task()
{
  AgsTaskLauncher *task_launcher;
  AgsTask *task_0, *task_1;
  
  task_launcher = ags_task_launcher_new();

  CU_ASSERT(task_launcher->task == NULL);
  
  task_0 = ags_task_new();
  ags_task_launcher_add_task(task_launcher, task_0);

  CU_ASSERT(g_list_find(task_launcher->task, task_0) != NULL);
  
  task_1 = ags_task_new();
  ags_task_launcher_add_task(task_launcher, task_1);

  CU_ASSERT(g_list_find(task_launcher->task, task_1) != NULL);
}

void
ags_task_launcher_test_add_task_all()
{
  AgsTaskLauncher *task_launcher;
  AgsTask *task_0, *task_1;

  GList *start_task;
  
  task_launcher = ags_task_launcher_new();

  start_task = NULL;
  
  task_0 = ags_task_new();
  start_task = g_list_prepend(start_task,
			      task_0);
  
  task_1 = ags_task_new();
  start_task = g_list_prepend(start_task,
			      task_1);

  ags_task_launcher_add_task_all(task_launcher, start_task);

  CU_ASSERT(g_list_find(task_launcher->task, task_0) != NULL);
  CU_ASSERT(g_list_find(task_launcher->task, task_1) != NULL);
}

void
ags_task_launcher_test_add_cyclic_task()
{
  AgsTaskLauncher *task_launcher;
  AgsTask *task_0, *task_1;
  
  task_launcher = ags_task_launcher_new();

  CU_ASSERT(task_launcher->cyclic_task == NULL);
  
  task_0 = ags_task_new();
  ags_task_launcher_add_cyclic_task(task_launcher, task_0);

  CU_ASSERT(g_list_find(task_launcher->cyclic_task, task_0) != NULL);
  
  task_1 = ags_task_new();
  ags_task_launcher_add_cyclic_task(task_launcher, task_1);

  CU_ASSERT(g_list_find(task_launcher->cyclic_task, task_1) != NULL);
}

void
ags_task_launcher_test_remove_cyclic_task()
{
  AgsTaskLauncher *task_launcher;
  AgsTask *task_0, *task_1;

  GList *start_task;
  
  task_launcher = ags_task_launcher_new();

  start_task = NULL;
  
  task_0 = ags_task_new();
  start_task = g_list_prepend(start_task,
			      task_0);
  
  task_1 = ags_task_new();
  start_task = g_list_prepend(start_task,
			      task_1);

  task_launcher->cyclic_task = start_task;
  
  ags_task_launcher_remove_cyclic_task(task_launcher, task_0);

  CU_ASSERT(g_list_find(task_launcher->cyclic_task, task_0) == NULL);
  
  ags_task_launcher_remove_cyclic_task(task_launcher, task_1);
  
  CU_ASSERT(g_list_find(task_launcher->cyclic_task, task_1) == NULL);
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
  pSuite = CU_add_suite("AgsTaskLauncherTest", ags_task_launcher_test_init_suite, ags_task_launcher_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsTaskLauncher test flags", ags_task_launcher_test_test_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTaskLauncher set flags", ags_task_launcher_test_set_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTaskLauncher unset flags", ags_task_launcher_test_unset_flags) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTaskLauncher add task", ags_task_launcher_test_add_task) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTaskLauncher add task all", ags_task_launcher_test_add_task_all) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTaskLauncher add cyclic task", ags_task_launcher_test_add_cyclic_task) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTaskLauncher remove cyclic task", ags_task_launcher_test_remove_cyclic_task) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

