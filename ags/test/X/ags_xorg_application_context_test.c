/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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
#include <ags/libags-audio.h>

#include <ags/X/ags_xorg_application_context.h>

#define AGS_XORG_APPLICATION_CONTEXT_TEST_DISPOSE_DEVOUT_COUNT (8)

int ags_xorg_application_context_test_init_suite();
int ags_xorg_application_context_test_clean_suite();

void ags_xorg_application_context_test_dispose();
void ags_xorg_application_context_test_finalize();

void ags_xorg_application_context_test_finalize_stub(GObject *gobject);

#define AGS_XORG_APPLICATION_CONTEXT_TEST_CONFIG "[generic]\n" \
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
  "\n"						       \
  "[soundcard-0]\n"				       \
  "backend=alsa\n"                                     \
  "device=hw:0,0\n"                                    \
  "samplerate=44100\n"				       \
  "buffer-size=1024\n"				       \
  "pcm-channels=2\n"				       \
  "dsp-channels=2\n"				       \
  "format=16\n"					       \
  "\n"						       \
  "[soundcard-1]\n"				       \
  "backend=alsa\n"                                     \
  "device=hw:0,0\n"                                    \
  "samplerate=44100\n"				       \
  "buffer-size=1024\n"				       \
  "pcm-channels=2\n"				       \
  "dsp-channels=2\n"				       \
  "format=16\n"					       \
  "\n"						       \
  "[recall]\n"					       \
  "auto-sense=true\n"				       \
  "\n"

gboolean xorg_application_context_test_finalized;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_xorg_application_context_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_xorg_application_context_test_clean_suite()
{
  
  return(0);
}

void
ags_xorg_application_context_test_dispose()
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsConfig *config;

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_XORG_APPLICATION_CONTEXT_TEST_CONFIG,
			    strlen(AGS_XORG_APPLICATION_CONTEXT_TEST_CONFIG));
    

  xorg_application_context = g_object_new(AGS_TYPE_XORG_APPLICATION_CONTEXT,
					  NULL);

  /* run dispose */
  g_object_run_dispose(xorg_application_context);

  /* assert */
  CU_ASSERT(xorg_application_context->thread_pool == NULL);
  CU_ASSERT(xorg_application_context->polling_thread == NULL);
  CU_ASSERT(xorg_application_context->soundcard_thread == NULL);
  CU_ASSERT(xorg_application_context->export_thread == NULL);
  CU_ASSERT(xorg_application_context->autosave_thread == NULL);
  CU_ASSERT(xorg_application_context->server == NULL);
  CU_ASSERT(xorg_application_context->soundcard == NULL);
  CU_ASSERT(xorg_application_context->sequencer == NULL);
  CU_ASSERT(xorg_application_context->distributed_manager == NULL);
  CU_ASSERT(xorg_application_context->window == NULL);
}

void
ags_xorg_application_context_test_finalize()
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsConfig *config;

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_XORG_APPLICATION_CONTEXT_TEST_CONFIG,
			    strlen(AGS_XORG_APPLICATION_CONTEXT_TEST_CONFIG));
  
  xorg_application_context = g_object_new(AGS_TYPE_XORG_APPLICATION_CONTEXT,
					   NULL);

  /* run dispose */
  g_object_run_dispose(xorg_application_context);

  /* stub finalize */
  xorg_application_context_test_finalized = FALSE;
  G_OBJECT_GET_CLASS(xorg_application_context)->finalize = ags_xorg_application_context_test_finalize_stub;

  /* unref and assert */
  g_object_unref(xorg_application_context);
  
  CU_ASSERT(xorg_application_context_test_finalized == TRUE);
}

void
ags_xorg_application_context_test_finalize_stub(GObject *gobject)
{
  xorg_application_context_test_finalized = TRUE;
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C\0");
  putenv("LANG=C\0");

  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsXorgApplicationContextTest\0", ags_xorg_application_context_test_init_suite, ags_xorg_application_context_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  gtk_init(NULL,
	   NULL);
  //  g_log_set_fatal_mask(G_LOG_DOMAIN, // "GLib-GObject\0", // "Gtk\0" G_LOG_DOMAIN,
  //		       G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING);

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsXorgApplicationContext doing dispose\0", ags_xorg_application_context_test_dispose) == NULL) ||
     (CU_add_test(pSuite, "test of AgsXorgApplicationContext doing finalize\0", ags_xorg_application_context_test_finalize) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
