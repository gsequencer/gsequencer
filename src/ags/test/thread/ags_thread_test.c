/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

int ags_thread_test_init_suite();
int ags_thread_test_clean_suite();

void ags_thread_test_sync();
void ags_thread_test_lock();
void ags_thread_test_trylock();
void ags_thread_test_get_toplevel();
void ags_thread_test_first();
void ags_thread_test_last();
void ags_thread_test_remove_child();
void ags_thread_test_add_child();
void ags_thread_test_parental_is_locked();
void ags_thread_test_sibling_is_locked();
void ags_thread_test_children_is_locked();
void ags_thread_test_is_current_ready();
void ags_thread_test_is_tree_ready();
void ags_thread_test_next_parent_locked();
void ags_thread_test_next_sibling_locked();
void ags_thread_test_next_children_locked();
void ags_thread_test_lock_parent();
void ags_thread_test_lock_sibling();
void ags_thread_test_lock_children();
void ags_thread_test_lock_all();
void ags_thread_test_wait_parent();
void ags_thread_test_wait_sibling();
void ags_thread_test_wait_children();
void ags_thread_test_suspend();
void ags_thread_test_timelock();
void ags_thread_test_stop();

AgsThread *main_loop;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_thread_test_init_suite()
{
  //TODO:JK: implement me
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_thread_test_clean_suite()
{
  //TODO:JK: implement me
  
  return(0);
}

void
ags_thread_test_sync()
{
  //TODO:JK: implement me
}

void
ags_thread_test_lock()
{
  //TODO:JK: implement me
}

void
ags_thread_test_trylock()
{
  //TODO:JK: implement me
}

void
ags_thread_test_get_toplevel()
{
  //TODO:JK: implement me
}

void
ags_thread_test_first()
{
  //TODO:JK: implement me
}

void
ags_thread_test_last()
{
  //TODO:JK: implement me
}

void
ags_thread_test_remove_child()
{
  //TODO:JK: implement me
}

void
ags_thread_test_add_child()
{
  //TODO:JK: implement me
}

void
ags_thread_test_parental_is_locked()
{
  //TODO:JK: implement me
}

void
ags_thread_test_sibling_is_locked()
{
  //TODO:JK: implement me
}

void
ags_thread_test_children_is_locked()
{
  //TODO:JK: implement me
}

void
ags_thread_test_is_current_ready()
{
  //TODO:JK: implement me
}

void
ags_thread_test_is_tree_ready()
{
  //TODO:JK: implement me
}

void
ags_thread_test_next_parent_locked()
{
  //TODO:JK: implement me
}

void
ags_thread_test_next_sibling_locked()
{
  //TODO:JK: implement me
}

void
ags_thread_test_next_children_locked()
{
  //TODO:JK: implement me
}

void
ags_thread_test_lock_parent()
{
  //TODO:JK: implement me
}

void
ags_thread_test_lock_sibling()
{
  //TODO:JK: implement me
}

void
ags_thread_test_lock_children()
{
  //TODO:JK: implement me
}

void
ags_thread_test_lock_all()
{
  //TODO:JK: implement me
}

void
ags_thread_test_wait_parent()
{
  //TODO:JK: implement me
}

void
ags_thread_test_wait_sibling()
{
  //TODO:JK: implement me
}

void
ags_thread_test_wait_children()
{
  //TODO:JK: implement me
}

void
ags_thread_test_suspend()
{
  //TODO:JK: implement me
}

void
ags_thread_test_timelock()
{
  //TODO:JK: implement me
}

void
ags_thread_test_stop()
{
  //TODO:JK: implement me
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
  pSuite = CU_add_suite("AgsThreadTest\0", ags_thread_test_init_suite, ags_thread_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsThread sync\0", ags_thread_test_sync) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread lock\0", ags_thread_test_lock) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread trylock\0", ags_thread_test_trylock) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread get toplevel\0", ags_thread_test_get_toplevel) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread first\0", ags_thread_test_first) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread last\0", ags_thread_test_last) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread remove child\0", ags_thread_test_remove_child) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread add child\0", ags_thread_test_add_child) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread parental is locked\0", ags_thread_test_parental_is_locked) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread sibling is locked\0", ags_thread_test_sibling_is_locked) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread children is locked\0", ags_thread_test_children_is_locked) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread is current ready\0", ags_thread_test_is_current_ready) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread is tree ready\0", ags_thread_test_is_tree_ready) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread next parent locked\0", ags_thread_test_next_parent_locked) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread next sibling locked\0", ags_thread_test_next_sibling_locked) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread next children locked\0", ags_thread_test_next_children_locked) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread lock parent\0", ags_thread_test_lock_parent) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread lock sibling\0", ags_thread_test_lock_sibling) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread lock childrend\0", ags_thread_test_lock_children) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread lock all\0", ags_thread_test_lock_all) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread wait parent\0", ags_thread_test_wait_parent) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread wait sibling\0", ags_thread_test_wait_sibling) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread wait children\0", ags_thread_test_wait_children) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread suspend\0", ags_thread_test_suspend) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread timelock\0", ags_thread_test_timelock) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread stop\0", ags_thread_test_stop) == NULL)){
      CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

