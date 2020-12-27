/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2020 Joël Krähemann
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

int ags_audio_ref_count_test_init_suite();
int ags_audio_ref_count_test_clean_suite();

void ags_audio_ref_count_test_recursive_set_property();
void ags_audio_ref_count_test_recursive_run_stage();

#define AGS_AUDIO_REF_COUNT_TEST_CONFIG "[generic]\n"	\
  "autosave-thread=false\n"				\
  "simple-file=true\n"					\
  "disable-feature=experimental\n"			\
  "segmentation=4/4\n"					\
  "\n"							\
  "[thread]\n"						\
  "model=super-threaded\n"				\
  "super-threaded-scope=channel\n"			\
  "lock-global=ags-thread\n"				\
  "lock-parent=ags-recycling-thread\n"			\
  "\n"							\
  "[soundcard]\n"					\
  "backend=alsa\n"					\
  "device=default\n"					\
  "samplerate=48000\n"					\
  "buffer-size=1024\n"					\
  "pcm-channels=2\n"					\
  "dsp-channels=2\n"					\
  "format=16\n"						\
  "\n"							\
  "[recall]\n"						\
  "auto-sense=true\n"					\
  "\n"

AgsAudioApplicationContext *audio_application_context;

GHashTable *object_ref_count;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_audio_ref_count_test_init_suite()
{
  AgsConfig *config;

  guint ref_count;

  object_ref_count = g_hash_table_new_full(g_direct_hash,
					   g_str_equal,
					   NULL,
					   NULL); 
  
  ags_priority_load_defaults(ags_priority_get_instance());  

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_AUDIO_REF_COUNT_TEST_CONFIG,
			    strlen(AGS_AUDIO_REF_COUNT_TEST_CONFIG));

  audio_application_context = (AgsApplicationContext *) ags_audio_application_context_new();
  g_object_ref(audio_application_context);

  ags_application_context_prepare(audio_application_context);
  ags_application_context_setup(audio_application_context);

  ref_count = g_atomic_int_get(&(G_OBJECT(audio_application_context)->ref_count));
  g_hash_table_insert(object_ref_count,
		      "/AgsSoundProvider",
		      GUINT_TO_POINTER(ref_count));

  ref_count = g_atomic_int_get(&(G_OBJECT(AGS_APPLICATION_CONTEXT(audio_application_context)->main_loop)->ref_count));
  g_hash_table_insert(object_ref_count,
		      "/AgsConcurrencyProvider/AgsMainLoop",
		      GUINT_TO_POINTER(ref_count));

  ref_count = g_atomic_int_get(&(G_OBJECT(AGS_APPLICATION_CONTEXT(audio_application_context)->task_launcher)->ref_count));
  g_hash_table_insert(object_ref_count,
		      "/AgsConcurrencyProvider/AgsTaskLauncher",
		      GUINT_TO_POINTER(ref_count));
  
  ref_count = g_atomic_int_get(&(G_OBJECT(audio_application_context->soundcard->data)->ref_count));
  g_hash_table_insert(object_ref_count,
		      "/AgsSoundProvider/AgsSoundcard[0]",
		      GUINT_TO_POINTER(ref_count));
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_audio_ref_count_test_clean_suite()
{  
  return(0);
}

void
ags_audio_ref_count_test_recursive_set_property()
{
  //TODO:JK: implement me
}

void
ags_audio_ref_count_test_recursive_run_stage()
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
  pSuite = CU_add_suite("AgsAudioRefCountTest", ags_audio_ref_count_test_init_suite, ags_audio_ref_count_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of audio objects ref count after recursive set property", ags_audio_ref_count_test_recursive_set_property) == NULL) ||
     (CU_add_test(pSuite, "test of audio objects ref count after recursive run stage", ags_audio_ref_count_test_recursive_run_stage) == NULL)){
      CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
