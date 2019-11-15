/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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
void ags_thread_test_is_current_ready();
void ags_thread_test_is_tree_ready();
void ags_thread_test_stop();

#define AGS_THREAD_TEST_SYNC_N_THREADS (16)

#define AGS_THREAD_TEST_LOCK_N_THREADS (4)

#define AGS_THREAD_TEST_GET_TOPLEVEL_N_LEVELS (7)

#define AGS_THREAD_TEST_FIRST_N_THREADS (16)

#define AGS_THREAD_TEST_LAST_N_THREADS (16)

#define AGS_THREAD_TEST_REMOVE_CHILD_N_THREADS (16)

#define AGS_THREAD_TEST_ADD_CHILD_N_THREADS (16)

AgsApplicationContext *application_context;

AgsThread *main_loop;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_thread_test_init_suite()
{
  application_context = ags_thread_application_context_new();
  g_object_ref(application_context);
  
  ags_application_context_prepare(application_context);
  ags_application_context_setup(application_context);
  
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  
  ags_thread_start(main_loop);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_thread_test_clean_suite()
{
  ags_thread_stop(main_loop);
  g_object_unref(main_loop);
  
  g_object_unref(application_context);
  
  return(0);
}

void
ags_thread_test_sync()
{
  AgsThread *main_loop;
  AgsThread **thread;

  volatile guint n_waiting;
  guint i;

  auto void* ags_thread_test_sync_waiter_thread(void *ptr);

  void* ags_thread_test_sync_waiter_thread(void *ptr){
    AgsThread *thread;

    thread = (AgsThread *) ptr;

    g_mutex_lock(&(thread->wait_mutex));

    g_atomic_int_inc(&n_waiting);
    
    while(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_WAIT_0) ||
	  ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_WAIT_1) ||
	  ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_WAIT_2)){
      g_cond_wait(&(thread->wait_cond),
		  &(thread->wait_mutex));
    }
    
    g_mutex_unlock(&(thread->wait_mutex));

    g_thread_exit(NULL);

    return(NULL);
  }
  
  main_loop = ags_generic_main_loop_new(application_context);
  g_atomic_int_set(&n_waiting,
		   0);
  
  thread = (AgsThread **) malloc(AGS_THREAD_TEST_SYNC_N_THREADS * sizeof(AgsThread*));
  
  for(i = 0; i < AGS_THREAD_TEST_SYNC_N_THREADS; i++){
    thread[i] = ags_thread_new(NULL);
    ags_thread_add_child_extended(main_loop,
				  thread[i],
				  TRUE, TRUE);
  }
  
  /* check AGS_THREAD_WAIT_0 - setup */
  ags_thread_set_status_flags(main_loop, AGS_THREAD_STATUS_WAIT_0);
  main_loop->thread = g_thread_new("libags.so - unit test",
				   ags_thread_test_sync_waiter_thread,
				   main_loop);

  for(i = 0; i < AGS_THREAD_TEST_SYNC_N_THREADS; i++){
    ags_thread_set_status_flags(thread[i], AGS_THREAD_STATUS_WAIT_0);

    /* since signal expects a thread waiting we do one */
    thread[i]->thread = g_thread_new("libags.so - unit test",
				     ags_thread_test_sync_waiter_thread,
				     thread[i]);
  }

  /* wait until all waiting */
  while(g_atomic_int_get(&n_waiting) < AGS_THREAD_TEST_SYNC_N_THREADS + 1){
    usleep(4);
  }

  /* call sync all */
  ags_thread_set_sync_all(main_loop,
			  0);

  /* assert flag not set anymore */
  CU_ASSERT(!ags_thread_test_status_flags(main_loop, AGS_THREAD_STATUS_WAIT_0));

  for(i = 0; i < AGS_THREAD_TEST_SYNC_N_THREADS; i++){
    CU_ASSERT(!ags_thread_test_status_flags(thread[i], AGS_THREAD_STATUS_WAIT_0));
  }
  
  /* check AGS_THREAD_WAIT_1 - setup */
  ags_thread_set_status_flags(main_loop, AGS_THREAD_STATUS_WAIT_1);
  main_loop->thread = g_thread_new("libags.so - unit test",
				   ags_thread_test_sync_waiter_thread,
				   main_loop);

  for(i = 0; i < AGS_THREAD_TEST_SYNC_N_THREADS; i++){
    ags_thread_set_status_flags(thread[i], AGS_THREAD_STATUS_WAIT_1);

    /* since signal expects a thread waiting we do one */
    thread[i]->thread = g_thread_new("libags.so - unit test",
				     ags_thread_test_sync_waiter_thread,
				     thread[i]);
  }

  /* wait until all waiting */
  while(g_atomic_int_get(&n_waiting) < AGS_THREAD_TEST_SYNC_N_THREADS + 1){
    usleep(4);
  }

  /* call sync all */
  ags_thread_set_sync_all(main_loop,
			  1);

  /* assert flag not set anymore */
  CU_ASSERT(!ags_thread_test_status_flags(main_loop, AGS_THREAD_STATUS_WAIT_1));

  for(i = 0; i < AGS_THREAD_TEST_SYNC_N_THREADS; i++){
    CU_ASSERT(!ags_thread_test_status_flags(thread[i], AGS_THREAD_STATUS_WAIT_1));
  }

  /* check AGS_THREAD_WAIT_2 - setup */
  ags_thread_set_status_flags(main_loop, AGS_THREAD_STATUS_WAIT_2);
  main_loop->thread = g_thread_new("libags.so - unit test",
				   ags_thread_test_sync_waiter_thread,
				   main_loop);

  for(i = 0; i < AGS_THREAD_TEST_SYNC_N_THREADS; i++){
    ags_thread_set_status_flags(thread[i], AGS_THREAD_STATUS_WAIT_2);

    /* since signal expects a thread waiting we do one */
    thread[i]->thread = g_thread_new("libags.so - unit test",
				     ags_thread_test_sync_waiter_thread,
				     thread[i]);
  }

  /* wait until all waiting */
  while(g_atomic_int_get(&n_waiting) < AGS_THREAD_TEST_SYNC_N_THREADS + 1){
    usleep(4);
  }

  /* call sync all */
  ags_thread_set_sync_all(main_loop,
			  2);

  /* assert flag not set anymore */
  CU_ASSERT(!ags_thread_test_status_flags(main_loop, AGS_THREAD_STATUS_WAIT_2));

  for(i = 0; i < AGS_THREAD_TEST_SYNC_N_THREADS; i++){
    CU_ASSERT(!ags_thread_test_status_flags(thread[i], AGS_THREAD_STATUS_WAIT_2));
  }
}

void
ags_thread_test_lock()
{
  AgsThread **thread;

  guint i;
  
  GThread *assert_thread;
  
  auto void* ags_thread_test_lock_assert_locked(void *ptr);

  void* ags_thread_test_lock_assert_locked(void *ptr){
    AgsThread **thread;

    guint i;

    thread = (AgsThread **) ptr;

    for(i = 0; i < AGS_THREAD_TEST_LOCK_N_THREADS; i++){
      CU_ASSERT(ags_thread_trylock(thread[i]) == FALSE);
    }

    g_thread_exit(NULL);

    return(NULL);
  }

  thread = (AgsThread **) malloc(AGS_THREAD_TEST_LOCK_N_THREADS * sizeof(AgsThread*));
  
  for(i = 0; i < AGS_THREAD_TEST_LOCK_N_THREADS; i++){
    thread[i] = ags_thread_new(NULL);
    ags_thread_add_child_extended(main_loop,
				  thread[i],
				  TRUE, TRUE);
  }

  /* lock the threads */
  for(i = 0; i < AGS_THREAD_TEST_LOCK_N_THREADS; i++){
    ags_thread_lock(thread[i]);
  }

  /* try to lock from another thread */
  assert_thread = g_thread_new("libags.so - unit test",
			       ags_thread_test_lock_assert_locked,
			       thread);

  g_thread_join(assert_thread);
  
  /* unlock the threads */
  for(i = 0; i < AGS_THREAD_TEST_LOCK_N_THREADS; i++){
    ags_thread_unlock(thread[i]);
  }
}

void
ags_thread_test_trylock()
{
  //TODO:JK: implement me
}

void
ags_thread_test_get_toplevel()
{
  AgsThread *main_loop;
  AgsThread *thread, *current;

  guint i;

  main_loop = ags_generic_main_loop_new(application_context);
  thread = main_loop;
  
  for(i = 0; i < AGS_THREAD_TEST_GET_TOPLEVEL_N_LEVELS; i++){
    current = ags_thread_new(NULL);
    ags_thread_add_child_extended(thread,
				  current,
				  TRUE, TRUE);

    thread = current;
  }

  CU_ASSERT(ags_thread_get_toplevel(thread) == main_loop);
}

void
ags_thread_test_first()
{
  AgsThread *parent;
  AgsThread **thread;
  AgsThread *first_thread;

  guint i;

  parent = ags_thread_new(NULL);
  ags_thread_add_child_extended(main_loop,
				parent,
				TRUE, TRUE);

  thread = (AgsThread **) malloc(AGS_THREAD_TEST_FIRST_N_THREADS * sizeof(AgsThread*));

  for(i = 0; i < AGS_THREAD_TEST_FIRST_N_THREADS; i++){
    thread[i] = ags_thread_new(NULL);
    ags_thread_add_child_extended(parent,
				  thread[i],
				  TRUE, TRUE);
  }

  first_thread = thread[0];
  
  for(i = 0; i < AGS_THREAD_TEST_FIRST_N_THREADS; i++){
    CU_ASSERT(ags_thread_first(thread[i]) == first_thread);
  }
}

void
ags_thread_test_last()
{
  AgsThread *parent;
  AgsThread **thread;
  AgsThread *last_thread;

  guint i;

  parent = ags_thread_new(NULL);
  ags_thread_add_child_extended(main_loop,
				parent,
				TRUE, TRUE);

  thread = (AgsThread **) malloc(AGS_THREAD_TEST_LAST_N_THREADS * sizeof(AgsThread*));

  for(i = 0; i < AGS_THREAD_TEST_LAST_N_THREADS; i++){
    thread[i] = ags_thread_new(NULL);
    ags_thread_add_child_extended(parent,
				  thread[i],
				  TRUE, TRUE);
  }

  last_thread = thread[AGS_THREAD_TEST_LAST_N_THREADS - 1];

  for(i = 0; i < AGS_THREAD_TEST_LAST_N_THREADS; i++){
    CU_ASSERT(ags_thread_last(thread[i]) == last_thread);
  }
}

void
ags_thread_test_remove_child()
{
  AgsThread *parent;
  AgsThread **thread;
  AgsThread *current, *next_current;

  guint i;

  parent = ags_thread_new(NULL);
  ags_thread_add_child_extended(main_loop,
				parent,
				TRUE, TRUE);

  thread = (AgsThread **) malloc(AGS_THREAD_TEST_REMOVE_CHILD_N_THREADS * sizeof(AgsThread*));

  for(i = 0; i < AGS_THREAD_TEST_REMOVE_CHILD_N_THREADS; i++){
    thread[i] = ags_thread_new(NULL);
    ags_thread_add_child_extended(parent,
				  thread[i],
				  TRUE, TRUE);
  }

  for(i = 0; i < AGS_THREAD_TEST_REMOVE_CHILD_N_THREADS; i++){
    g_object_ref(thread[i]);
    ags_thread_remove_child(parent,
			    thread[i]);

    current = ags_thread_children(parent);

    while(current != NULL){
      CU_ASSERT(current != thread[i]);

      /* iterate */
      next_current = ags_thread_next(current);

      g_object_unref(current);

      current = next_current;
    }

    CU_ASSERT(ags_thread_parent(thread[i]) == NULL);
    
    g_object_unref(thread[i]);
  }
}

void
ags_thread_test_add_child()
{
  AgsThread *parent;
  AgsThread **thread;
  AgsThread *current, *next_current;

  guint i;
  gboolean success;

  parent = ags_thread_new(NULL);
  ags_thread_add_child_extended(main_loop,
				parent,
				TRUE, TRUE);

  thread = (AgsThread **) malloc(AGS_THREAD_TEST_ADD_CHILD_N_THREADS * sizeof(AgsThread*));

  for(i = 0; i < AGS_THREAD_TEST_ADD_CHILD_N_THREADS; i++){
    thread[i] = ags_thread_new(NULL);
    ags_thread_add_child_extended(parent,
				  thread[i],
				  TRUE, TRUE);

    current = ags_thread_children(parent);

    success = FALSE;
    
    while(current != NULL){
      if(current = thread[i]){
	success = TRUE;
	
	break;
      }
      
      /* iterate */
      next_current = ags_thread_next(current);

      g_object_unref(current);

      current = next_current;
    }

    CU_ASSERT(success);
    CU_ASSERT(ags_thread_parent(thread[i]) == parent);
  }
}

void
ags_thread_test_is_current_ready()
{
  AgsThread *parent;
  AgsThread *thread;

  parent = ags_thread_new(NULL);
  
  thread = ags_thread_new(NULL);
  ags_thread_add_child_extended(parent,
				thread,
				TRUE, TRUE);

  /* not ready at all */
  CU_ASSERT(ags_thread_is_current_ready(parent, 0) == TRUE);
  CU_ASSERT(ags_thread_is_current_ready(thread, 0) == TRUE);

  CU_ASSERT(ags_thread_is_current_ready(parent, 1) == TRUE);
  CU_ASSERT(ags_thread_is_current_ready(thread, 1) == TRUE);

  CU_ASSERT(ags_thread_is_current_ready(parent, 2) == TRUE);
  CU_ASSERT(ags_thread_is_current_ready(thread, 2) == TRUE);

  /* wait 0 ready */
  ags_thread_set_status_flags(parent, AGS_THREAD_STATUS_WAIT_0);
  ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_WAIT_0);

  CU_ASSERT(ags_thread_is_current_ready(parent, 0) == TRUE);
  CU_ASSERT(ags_thread_is_current_ready(thread, 0) == TRUE);

  ags_thread_unset_status_flags(parent, AGS_THREAD_STATUS_WAIT_0);
  ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_WAIT_0);

  /* wait 1 ready */
  ags_thread_set_status_flags(parent, AGS_THREAD_STATUS_WAIT_1);
  ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_WAIT_1);

  CU_ASSERT(ags_thread_is_current_ready(parent, 1) == TRUE);
  CU_ASSERT(ags_thread_is_current_ready(thread, 1) == TRUE);

  ags_thread_unset_status_flags(parent, AGS_THREAD_STATUS_WAIT_1);
  ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_WAIT_1);

  /* wait 2 ready */
  ags_thread_set_status_flags(parent, AGS_THREAD_STATUS_WAIT_2);
  ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_WAIT_2);

  CU_ASSERT(ags_thread_is_current_ready(parent, 2) == TRUE);
  CU_ASSERT(ags_thread_is_current_ready(thread, 2) == TRUE);

  ags_thread_unset_status_flags(parent, AGS_THREAD_STATUS_WAIT_2);
  ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_WAIT_2);
}

void
ags_thread_test_is_tree_ready()
{
  AgsThread *parent;
  AgsThread *thread;

  parent = ags_thread_new(NULL);
  
  thread = ags_thread_new(NULL);
  ags_thread_add_child_extended(parent,
				thread,
				TRUE, TRUE);

  /* not ready at all */
  CU_ASSERT(ags_thread_is_tree_ready(parent, 0) == TRUE);

  CU_ASSERT(ags_thread_is_tree_ready(parent, 1) == TRUE);

  CU_ASSERT(ags_thread_is_tree_ready(parent, 2) == TRUE);

  /* wait 0 ready */
  ags_thread_set_status_flags(parent, AGS_THREAD_STATUS_WAIT_0);
  ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_WAIT_0);

  CU_ASSERT(ags_thread_is_tree_ready(parent, 0) == TRUE);

  ags_thread_unset_status_flags(parent, AGS_THREAD_STATUS_WAIT_0);
  ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_WAIT_0);

  /* wait 1 ready */
  ags_thread_set_status_flags(parent, AGS_THREAD_STATUS_WAIT_1);
  ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_WAIT_1);

  CU_ASSERT(ags_thread_is_tree_ready(parent, 1) == TRUE);

  ags_thread_unset_status_flags(parent, AGS_THREAD_STATUS_WAIT_1);
  ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_WAIT_1);

  /* wait 2 ready */
  ags_thread_set_status_flags(parent, AGS_THREAD_STATUS_WAIT_2);
  ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_WAIT_2);

  CU_ASSERT(ags_thread_is_tree_ready(parent, 2) == TRUE);

  ags_thread_unset_status_flags(parent, AGS_THREAD_STATUS_WAIT_2);
  ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_WAIT_2);
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

  putenv("LC_ALL=C");
  putenv("LANG=C");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsThreadTest", ags_thread_test_init_suite, ags_thread_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if(/* (CU_add_test(pSuite, "test of AgsThread sync", ags_thread_test_sync) == NULL) || - needs fix because of a race-condition */
     (CU_add_test(pSuite, "test of AgsThread lock", ags_thread_test_lock) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread trylock", ags_thread_test_trylock) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread get toplevel", ags_thread_test_get_toplevel) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread first", ags_thread_test_first) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread last", ags_thread_test_last) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread remove child", ags_thread_test_remove_child) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread add child", ags_thread_test_add_child) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread is current ready", ags_thread_test_is_current_ready) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread is tree ready", ags_thread_test_is_tree_ready) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThread stop", ags_thread_test_stop) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

