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
#include <CUnit/Basic.h>

#include <ags/libags.h>

int ags_thread_application_context_test_init_suite();
int ags_thread_application_context_test_clean_suite();

void ags_thread_application_context_test_dispose();
void ags_thread_application_context_test_finalize();

void ags_thread_application_context_test_finalize_stub(GObject *gobject);

#define AGS_THREAD_APPLICATION_CONTEXT_TEST_CONFIG "[generic]\n" \
  "autosave-thread=false\n"			       \
  "simple-file=true\n"				       \
  "disable-feature=experimental\n"		       \
  "segmentation=4/4\n"				       \
  "\n"						       \
  "[thread]\n"					       \
  "model=super-threaded\n"			       \
  "super-threaded-scope=channel\n"		       \
  "lock-global=ags-thread\n"			       \
  "lock-parent=ags-recycling-thread\n"		       \
  "\n"

gboolean thread_application_context_test_finalized;

extern AgsApplicationContext *ags_application_context;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_thread_application_context_test_init_suite()
{    
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_thread_application_context_test_clean_suite()
{
  
  return(0);
}

void
ags_thread_application_context_test_dispose()
{
  AgsThreadApplicationContext *thread_application_context;

  AgsConfig *config;

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_THREAD_APPLICATION_CONTEXT_TEST_CONFIG,
			    strlen(AGS_THREAD_APPLICATION_CONTEXT_TEST_CONFIG));

  thread_application_context = g_object_new(AGS_TYPE_THREAD_APPLICATION_CONTEXT,
					   NULL);
  g_object_ref(thread_application_context);
  
  /* run dispose */
  g_object_run_dispose(thread_application_context);

  /* assert */
  CU_ASSERT(thread_application_context->thread_pool == NULL);
  CU_ASSERT(thread_application_context->worker == NULL);

  ags_application_context = NULL;
}

void
ags_thread_application_context_test_finalize()
{
  AgsThreadApplicationContext *thread_application_context;

  AgsThread *main_loop;
  GThread *thread;
  
  AgsConfig *config;

  GList *list;
  
  gpointer gobject_class;
  gpointer orig_finalize;
  
  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_THREAD_APPLICATION_CONTEXT_TEST_CONFIG,
			    strlen(AGS_THREAD_APPLICATION_CONTEXT_TEST_CONFIG));

  thread_application_context = g_object_new(AGS_TYPE_THREAD_APPLICATION_CONTEXT,
					   NULL);

  /* run dispose */  
  g_object_run_dispose(thread_application_context);

  /* stub finalize */
  thread_application_context_test_finalized = FALSE;

  gobject_class = G_OBJECT_GET_CLASS(thread_application_context);

  orig_finalize = G_OBJECT_CLASS(gobject_class)->finalize;
  
  G_OBJECT_CLASS(gobject_class)->finalize = ags_thread_application_context_test_finalize_stub;
  
  /* unref and assert */
  g_object_unref(thread_application_context);
  
  CU_ASSERT(thread_application_context_test_finalized == TRUE);
  
  G_OBJECT_CLASS(gobject_class)->finalize = orig_finalize;

  ags_application_context = NULL;
}

void
ags_thread_application_context_test_finalize_stub(GObject *gobject)
{
  thread_application_context_test_finalized = TRUE;
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
  pSuite = CU_add_suite("AgsThreadApplicationContextTest", ags_thread_application_context_test_init_suite, ags_thread_application_context_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsThreadApplicationContext doing dispose", ags_thread_application_context_test_dispose) == NULL) ||
     (CU_add_test(pSuite, "test of AgsThreadApplicationContext doing finalize", ags_thread_application_context_test_finalize) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
