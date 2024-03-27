/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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
#include <ags/libags-audio.h>

#include <math.h>

int ags_main_loop_test_init_suite();
int ags_main_loop_test_clean_suite();

void ags_main_loop_test_get_tree_lock();
void ags_main_loop_test_set_syncing();
void ags_main_loop_test_is_syncing();
void ags_main_loop_test_set_critical_region();
void ags_main_loop_test_is_critical_region();
void ags_main_loop_test_inc_queued_critical_region();
void ags_main_loop_test_dec_queued_critical_region();
void ags_main_loop_test_test_queued_critical_region();

void ags_main_loop_test_change_frequency();

GType main_loop_test_types[3];
 
/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_main_loop_test_init_suite()
{
  guint i;

  ags_audio_application_context_new();
  
  i = 0;
  
  main_loop_test_types[i++] = AGS_TYPE_GENERIC_MAIN_LOOP;
  main_loop_test_types[i++] = AGS_TYPE_AUDIO_LOOP;
  main_loop_test_types[i++] = G_TYPE_NONE;

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_main_loop_test_clean_suite()
{  
  return(0);
}

void
ags_main_loop_test_get_tree_lock()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; main_loop_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(main_loop_test_types[i],
			   NULL);

    if(AGS_MAIN_LOOP_GET_INTERFACE(AGS_MAIN_LOOP(current))->get_tree_lock == NULL){
      g_message("AgsMainLoop::get-tree-lock missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_main_loop_get_tree_lock(current);
  }

  CU_ASSERT(success);
}

void
ags_main_loop_test_set_syncing()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; main_loop_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(main_loop_test_types[i],
			   NULL);

    if(AGS_MAIN_LOOP_GET_INTERFACE(AGS_MAIN_LOOP(current))->set_syncing == NULL){
      g_message("AgsMainLoop::set-syncing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_main_loop_set_syncing(current,
			      FALSE);
  }

  CU_ASSERT(success);
}

void
ags_main_loop_test_is_syncing()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; main_loop_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(main_loop_test_types[i],
			   NULL);

    if(AGS_MAIN_LOOP_GET_INTERFACE(AGS_MAIN_LOOP(current))->is_syncing == NULL){
      g_message("AgsMainLoop::is-syncing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_main_loop_is_syncing(current);
  }

  CU_ASSERT(success);
}

void
ags_main_loop_test_set_critical_region()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; main_loop_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(main_loop_test_types[i],
			   NULL);

    if(AGS_MAIN_LOOP_GET_INTERFACE(AGS_MAIN_LOOP(current))->set_critical_region == NULL){
      g_message("AgsMainLoop::set-critical-region missing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_main_loop_set_critical_region(current,
				      FALSE);
  }

  CU_ASSERT(success);
}

void
ags_main_loop_test_is_critical_region()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; main_loop_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(main_loop_test_types[i],
			   NULL);

    if(AGS_MAIN_LOOP_GET_INTERFACE(AGS_MAIN_LOOP(current))->is_critical_region == NULL){
      g_message("AgsMainLoop::is-critical-region: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_main_loop_is_critical_region(current);
  }

  CU_ASSERT(success);
}

void
ags_main_loop_test_change_frequency()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; main_loop_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(main_loop_test_types[i],
			   NULL);

    if(AGS_MAIN_LOOP_GET_INTERFACE(AGS_MAIN_LOOP(current))->change_frequency == NULL){
      g_message("AgsMainLoop::set-syncing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_main_loop_change_frequency(current,
				   250.0);
  }

  CU_ASSERT(success);
}

void
ags_main_loop_test_inc_queued_critical_region()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; main_loop_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(main_loop_test_types[i],
			   NULL);

    if(AGS_MAIN_LOOP_GET_INTERFACE(AGS_MAIN_LOOP(current))->inc_queued_critical_region == NULL){
      g_message("AgsMainLoop::set-syncing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_main_loop_inc_queued_critical_region(current);
  }

  CU_ASSERT(success);
}

void
ags_main_loop_test_dec_queued_critical_region()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; main_loop_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(main_loop_test_types[i],
			   NULL);

    if(AGS_MAIN_LOOP_GET_INTERFACE(AGS_MAIN_LOOP(current))->dec_queued_critical_region == NULL){
      g_message("AgsMainLoop::set-syncing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_main_loop_inc_queued_critical_region(current);
    ags_main_loop_dec_queued_critical_region(current);
  }

  CU_ASSERT(success);
}

void
ags_main_loop_test_test_queued_critical_region()
{
  GObject *current;
  
  GType current_type;

  guint i;
  gboolean success;

  success = TRUE;
  
  for(i = 0; main_loop_test_types[i] != G_TYPE_NONE; i++){
    current = g_object_new(main_loop_test_types[i],
			   NULL);

    if(AGS_MAIN_LOOP_GET_INTERFACE(AGS_MAIN_LOOP(current))->test_queued_critical_region == NULL){
      g_message("AgsMainLoop::set-syncing: %s", G_OBJECT_TYPE_NAME(current));
      
      success = FALSE;
    }
    
    ags_main_loop_test_queued_critical_region(current);
  }

  CU_ASSERT(success);
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

  /* remove a suite to the registry */
  pSuite = CU_add_suite("AgsMainLoopTest", ags_main_loop_test_init_suite, ags_main_loop_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* remove the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsMainLoop get tree lock", ags_main_loop_test_get_tree_lock) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMainLoop set syncing", ags_main_loop_test_set_syncing) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMainLoop is syncing", ags_main_loop_test_is_syncing) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMainLoop set critical region", ags_main_loop_test_set_critical_region) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMainLoop is critical region", ags_main_loop_test_is_critical_region) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMainLoop change frequency", ags_main_loop_test_change_frequency) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
