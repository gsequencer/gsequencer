/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2018 Joël Krähemann
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

int ags_devin_test_init_suite();
int ags_devin_test_clean_suite();

void ags_devin_test_dispose();
void ags_devin_test_finalize();

void ags_devin_test_finalize_stub(GObject *gobject);

#define AGS_DEVIN_TEST_CONFIG "[generic]\n" \
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
  "[recall]\n"					       \
  "auto-sense=true\n"				       \
  "\n"


AgsAudioApplicationContext *audio_application_context;
gboolean devin_test_finalized;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_devin_test_init_suite()
{
  AgsConfig *config;

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_DEVIN_TEST_CONFIG,
			    strlen(AGS_DEVIN_TEST_CONFIG));
  
  audio_application_context = ags_audio_application_context_new();
  g_object_ref(audio_application_context);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_devin_test_clean_suite()
{
  g_object_unref(audio_application_context);
  
  return(0);
}

void
ags_devin_test_dispose()
{
  AgsDevin *devin;
  AgsAudio *audio;
  
  GList *list, *list_start;
  
  guint i;
  gboolean success;
  
  devin = g_object_new(AGS_TYPE_DEVIN,
			"application-context", audio_application_context,
			NULL);
  g_object_ref(devin);
  

  /* run dispose */
  g_object_run_dispose(devin);

  /* assert no application context */
  CU_ASSERT(ags_soundcard_get_application_context(AGS_SOUNDCARD(devin)) == NULL);
  
  /* assert */
  CU_ASSERT(success == TRUE);
}

void
ags_devin_test_finalize()
{
  AgsDevin *devin;
  AgsAudio *audio;
    
  guint i;
  
  devin = g_object_new(AGS_TYPE_DEVIN,
			"application-context", audio_application_context,
			NULL);

  /* run dispose */
  g_object_run_dispose(devin);

  /* stub finalize */
  devin_test_finalized = FALSE;
  G_OBJECT_GET_CLASS(devin)->finalize = ags_devin_test_finalize_stub;

  /* unref and assert */
  g_object_unref(devin);
  
  CU_ASSERT(devin_test_finalized == TRUE);
}

void
ags_devin_test_finalize_stub(GObject *gobject)
{
  devin_test_finalized = TRUE;
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
  pSuite = CU_add_suite("AgsDevinTest", ags_devin_test_init_suite, ags_devin_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsDevin doing dispose", ags_devin_test_dispose) == NULL) ||
     (CU_add_test(pSuite, "test of AgsDevin doing finalize", ags_devin_test_finalize) == NULL)){
      CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
