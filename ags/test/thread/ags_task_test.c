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

int ags_task_test_init_suite();
int ags_task_test_clean_suite();

void ags_task_test_launch();
void ags_task_test_failure();

void ags_task_test_stub_launch(AgsTask *task);
void ags_task_test_stub_failure(AgsTask *task, GError *error);

gboolean stub_launch = FALSE;
gboolean stub_failure = FALSE;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_task_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_task_test_clean_suite()
{
  return(0);
}

void
ags_task_test_launch()
{
  AgsTask *task;
  gpointer *ptr;
  
  task = g_object_new(AGS_TYPE_TASK,
		      NULL);

  /* stub launch */
  ptr = AGS_TASK_GET_CLASS(task)->launch;
  AGS_TASK_GET_CLASS(task)->launch = ags_task_test_stub_launch;

  /* launch and assert */
  ags_task_launch(task);

  CU_ASSERT(stub_launch == TRUE);
  
  AGS_TASK_GET_CLASS(task)->launch = ptr;
}

void
ags_task_test_failure()
{
  AgsTask *task;
  gpointer *ptr;

  GError *error;
  
  task = g_object_new(AGS_TYPE_TASK,
		      NULL);

  /* stub failure */
  ptr = AGS_TASK_GET_CLASS(task)->failure;
  AGS_TASK_GET_CLASS(task)->failure = ags_task_test_stub_failure;

  /* failure and assert */
  error = NULL;
  ags_task_failure(task,
		   &error);

  CU_ASSERT(stub_failure == TRUE);
  
  AGS_TASK_GET_CLASS(task)->failure = ptr;
}

void
ags_task_test_stub_launch(AgsTask *task)
{
  stub_launch = TRUE;
}

void
ags_task_test_stub_failure(AgsTask *task, GError *error)
{
  stub_failure = TRUE;
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
  pSuite = CU_add_suite("AgsTaskTest\0", ags_task_test_init_suite, ags_task_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsTask launch\0", ags_task_test_launch) == NULL) ||
     (CU_add_test(pSuite, "test of AgsTask failure\0", ags_task_test_failure) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
