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

int ags_polling_thread_test_init_suite();
int ags_polling_thread_test_clean_suite();

void ags_polling_thread_test_fd_position();
void ags_polling_thread_test_add_poll_fd();
void ags_polling_thread_test_remove_poll_fd();

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_polling_thread_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_polling_thread_test_clean_suite()
{
  return(0);
}

void
ags_polling_thread_test_fd_position()
{
  AgsPollingThread *polling_thread;
  AgsPollFd *poll_fd;

  GList *list;
  
  polling_thread = g_object_new(AGS_TYPE_POLLING_THREAD,
				NULL);

  /* create poll fd */
  list = NULL;
  
  poll_fd = g_object_new(AGS_TYPE_POLL_FD,
			 NULL);
  list = g_list_prepend(list,
			poll_fd);

  poll_fd = g_object_new(AGS_TYPE_POLL_FD,
			 NULL);
  list = g_list_prepend(list,
			poll_fd);

  poll_fd = g_object_new(AGS_TYPE_POLL_FD,
			 NULL);
  list = g_list_prepend(list,
			poll_fd);

  polling_thread->poll_fd = g_list_reverse(list);
  polling_thread->fds = (struct pollfd *) malloc(3 * sizeof(struct pollfd));

  polling_thread->fds[0].fd = 0;
  polling_thread->fds[1].fd = 1;
  polling_thread->fds[2].fd = 2;

  /* assert position */
  CU_ASSERT(ags_polling_thread_fd_position(polling_thread,
					   0) == 0);

  CU_ASSERT(ags_polling_thread_fd_position(polling_thread,
					   1) == 1);

  CU_ASSERT(ags_polling_thread_fd_position(polling_thread,
					   2) == 2);
}

void
ags_polling_thread_test_add_poll_fd()
{
  AgsPollingThread *polling_thread;
  AgsPollFd *poll_fd[3];
  
  polling_thread = g_object_new(AGS_TYPE_POLLING_THREAD,
				NULL);

  /* add poll fd */
  poll_fd[0] = g_object_new(AGS_TYPE_POLL_FD,
			    NULL);
  poll_fd[0]->fd = 0;
  ags_polling_thread_add_poll_fd(polling_thread,
				 poll_fd[0]);

  poll_fd[1] = g_object_new(AGS_TYPE_POLL_FD,
			    NULL);
  poll_fd[1]->fd = 1;
  ags_polling_thread_add_poll_fd(polling_thread,
				 poll_fd[1]);

  poll_fd[2] = g_object_new(AGS_TYPE_POLL_FD,
			    NULL);
  poll_fd[2]->fd = 2;
  ags_polling_thread_add_poll_fd(polling_thread,
				 poll_fd[2]);

  /* assert */
  CU_ASSERT(g_list_find(polling_thread->poll_fd,
			poll_fd[0]) != NULL &&
	    polling_thread->fds[0].fd == 0);

  CU_ASSERT(g_list_find(polling_thread->poll_fd,
			poll_fd[1]) != NULL &&
	    polling_thread->fds[1].fd == 1);

  CU_ASSERT(g_list_find(polling_thread->poll_fd,
			poll_fd[2]) != NULL &&
	    polling_thread->fds[2].fd == 2);
}

void
ags_polling_thread_test_remove_poll_fd()
{
  AgsPollingThread *polling_thread;
  AgsPollFd *poll_fd[3];

  GList *list;
  
  polling_thread = g_object_new(AGS_TYPE_POLLING_THREAD,
				NULL);

  /* create poll fd */
  list = NULL;
  
  poll_fd[0] = g_object_new(AGS_TYPE_POLL_FD,
			    NULL);
  list = g_list_prepend(list,
			poll_fd[0]);

  poll_fd[1] = g_object_new(AGS_TYPE_POLL_FD,
			    NULL);
  list = g_list_prepend(list,
			poll_fd[1]);

  poll_fd[2] = g_object_new(AGS_TYPE_POLL_FD,
			    NULL);
  list = g_list_prepend(list,
			poll_fd[2]);

  polling_thread->poll_fd = g_list_reverse(list);
  polling_thread->fds = (struct pollfd *) malloc(3 * sizeof(struct pollfd));
  
  polling_thread->fds[0].fd = 0;
  polling_thread->fds[1].fd = 1;
  polling_thread->fds[2].fd = 2;

  /* assert remove */
  ags_polling_thread_remove_poll_fd(polling_thread,
				    poll_fd[0]);
  CU_ASSERT(g_list_find(polling_thread->poll_fd,
			poll_fd[0]) == NULL);
  
  ags_polling_thread_remove_poll_fd(polling_thread,
				    poll_fd[1]);
  CU_ASSERT(g_list_find(polling_thread->poll_fd,
			poll_fd[1]) == NULL);
  
  ags_polling_thread_remove_poll_fd(polling_thread,
				    poll_fd[2]);
  CU_ASSERT(polling_thread->poll_fd == NULL);
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
  pSuite = CU_add_suite("AgsPollingThreadTest\0", ags_polling_thread_test_init_suite, ags_polling_thread_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsPollingThread fd position\0", ags_polling_thread_test_fd_position) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPollingThread add poll fd\0", ags_polling_thread_test_add_poll_fd) == NULL) ||
     (CU_add_test(pSuite, "test of AgsPollingThread remove poll fd\0", ags_polling_thread_test_remove_poll_fd) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
