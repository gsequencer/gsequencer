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

int ags_poll_fd_test_init_suite();
int ags_poll_fd_test_clean_suite();

void ags_poll_fd_test_dispatch();

void ags_poll_fd_test_stub_dispatch(AgsPollFd *poll_fd);

gboolean stub_dispatch = FALSE;

/* The suite initialization time.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_poll_fd_test_init_suite()
{
  return(0);
}

/* The suite cleanup time.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_poll_fd_test_clean_suite()
{
  return(0);
}

void
ags_poll_fd_test_dispatch()
{
  AgsPollFd *poll_fd;

  gpointer ptr;
  
  poll_fd = g_object_new(AGS_TYPE_POLL_FD,
			 NULL);

  ptr = AGS_POLL_FD_GET_CLASS(poll_fd)->dispatch;
  AGS_POLL_FD_GET_CLASS(poll_fd)->dispatch = ags_poll_fd_test_stub_dispatch;

  /* assert change program */
  ags_poll_fd_dispatch(poll_fd);

  CU_ASSERT(stub_dispatch == TRUE);
  AGS_POLL_FD_GET_CLASS(poll_fd)->dispatch = ptr;
}

void
ags_poll_fd_test_stub_dispatch(AgsPollFd *poll_fd)
{
  stub_dispatch = TRUE;
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
  pSuite = CU_add_suite("AgsPollFdTest\0", ags_poll_fd_test_init_suite, ags_poll_fd_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsPollFd dispatch\0", ags_poll_fd_test_dispatch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
