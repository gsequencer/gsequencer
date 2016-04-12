/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2016 Joël Krähemann
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

#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall.h>

int ags_recall_test_init_suite();
int ags_recall_test_clean_suite();

void ags_recall_test_resolve_dependencies();
void ags_recall_test_child_added();
void ags_recall_test_run_init_pre();
void ags_recall_test_run_init_inter();
void ags_recall_test_run_init_post();
void ags_recall_test_stop_persistent();
void ags_recall_test_done();
void ags_recall_test_cancel();
void ags_recall_test_remove();
void ags_recall_test_is_done();
void ags_recall_test_duplicate();
void ags_recall_test_set_recall_id();
void ags_recall_test_set_soundcard_recursive();
void ags_recall_test_notify_dependency();
void ags_recall_test_add_dependency();
void ags_recall_test_remove_dependency();
void ags_recall_test_get_dependencies();
void ags_recall_test_remove_child();
void ags_recall_test_add_child();
void ags_recall_test_get_children();
void ags_recall_test_get_by_effect();
void ags_recall_test_find_by_effect();
void ags_recall_test_find_type();
void ags_recall_test_find_template();
void ags_recall_test_template_find_type();
void ags_recall_test_template_find_all_type();
void ags_recall_test_find_type_with_recycling_context();
void ags_recall_test_find_recycling_context();
void ags_recall_test_find_provider();
void ags_recall_test_template_find_provider();
void ags_recall_test_find_provider_with_recycling_context();
void ags_recall_test_run_init();
void ags_recall_test_add_handler();
void ags_recall_test_remove_handler();
void ags_recall_test_lock_port();
void ags_recall_test_unlock_port();

AgsApplicationContext *application_context;
AgsDevout *devout;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_test_clean_suite()
{
  return(0);
}

void
ags_recall_test_resolve_dependencies()
{
  CU_ASSERT(TRUE);
}

void
ags_recall_test_child_added()
{
}

void
ags_recall_test_run_init_pre()
{
}

void
ags_recall_test_run_init_inter()
{
}

void
ags_recall_test_run_init_post()
{
}

void
ags_recall_test_stop_persistent()
{
}

void
ags_recall_test_done()
{
}

void
ags_recall_test_cancel()
{
}

void
ags_recall_test_remove()
{
}

void
ags_recall_test_is_done()
{
}

void
ags_recall_test_duplicate()
{
}

void
ags_recall_test_set_recall_id()
{
}

void
ags_recall_test_set_soundcard_recursive()
{
}

void
ags_recall_test_notify_dependency()
{
}

void
ags_recall_test_add_dependency()
{
}

void
ags_recall_test_remove_dependency()
{
}

void
ags_recall_test_get_dependencies()
{
}

void
ags_recall_test_remove_child()
{
}

void
ags_recall_test_add_child()
{
}

void
ags_recall_test_get_children()
{
}

void
ags_recall_test_get_by_effect()
{
}

void
ags_recall_test_find_by_effect()
{
}

void
ags_recall_test_find_type()
{
}

void
ags_recall_test_find_template()
{
}

void
ags_recall_test_template_find_type()
{
}

void
ags_recall_test_template_find_all_type()
{
}

void
ags_recall_test_find_type_with_recycling_context()
{
}

void
ags_recall_test_find_recycling_context()
{
}

void
ags_recall_test_find_provider()
{
}

void
ags_recall_test_template_find_provider()
{
}

void
ags_recall_test_find_provider_with_recycling_context()
{
}

void
ags_recall_test_run_init()
{
}

void
ags_recall_test_add_handler()
{
}

void
ags_recall_test_remove_handler()
{
}

void
ags_recall_test_lock_port()
{
}

void
ags_recall_test_unlock_port()
{
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
  pSuite = CU_add_suite("AgsRecallTest\0", ags_recall_test_init_suite, ags_recall_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsRecall resolve dependencies\0", ags_recall_test_resolve_dependencies) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall child added\0", ags_recall_test_child_added) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall run init pre\0", ags_recall_test_run_init_pre) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall run init inter\0", ags_recall_test_run_init_inter) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall run init post\0", ags_recall_test_run_init_post) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall stop persistent\0", ags_recall_test_stop_persistent) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall done\0", ags_recall_test_done) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall cancel\0", ags_recall_test_cancel) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall remove\0", ags_recall_test_remove) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall is done\0", ags_recall_test_is_done) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall duplicate\0", ags_recall_test_duplicate) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall set recall id\0", ags_recall_test_set_recall_id) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall set soundcard recursive\0", ags_recall_test_set_soundcard_recursive) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall notify dependency\0", ags_recall_test_notify_dependency) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall add dependency\0", ags_recall_test_add_dependency) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall remove dependency\0", ags_recall_test_remove_dependency) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall get dependencies\0", ags_recall_test_get_dependencies) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall remove child\0", ags_recall_test_remove_child) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall add child\0", ags_recall_test_add_child) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall get children\0", ags_recall_test_get_children) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall get by effect\0", ags_recall_test_get_by_effect) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find type\0", ags_recall_test_find_type) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find template\0", ags_recall_test_find_template) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall template find type\0", ags_recall_test_template_find_type) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall template find all type\0", ags_recall_test_template_find_all_type) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find type with recycling context\0", ags_recall_test_find_type_with_recycling_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find recycling context\0", ags_recall_test_find_recycling_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find provider\0", ags_recall_test_find_provider) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall template find provider\0", ags_recall_test_template_find_provider) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall find provider with recycling context\0", ags_recall_test_find_provider_with_recycling_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall run init\0", ags_recall_test_run_init) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall add handler\0", ags_recall_test_add_handler) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall remove handler\0", ags_recall_test_remove_handler) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall lock port\0", ags_recall_test_lock_port) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecall unlock port\0", ags_recall_test_unlock_port) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

