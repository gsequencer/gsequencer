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

#define AGS_MIDIIN_TEST_DISPOSE_AUDIO_COUNT (8)

#define AGS_MIDIIN_TEST_FINALIZE_AUDIO_COUNT (8)

int ags_midiin_test_init_suite();
int ags_midiin_test_clean_suite();

void ags_midiin_test_dispose();
void ags_midiin_test_finalize();

void ags_midiin_test_finalize_stub(GObject *gobject);

#define AGS_MIDIIN_TEST_CONFIG "[generic]\n"	\
  "autosave-thread=false\n"			\
  "simple-file=true\n"				\
  "disable-feature=experimental\n"		\
  "segmentation=4/4\n"				\
  "\n"						\
  "[thread]\n"					\
  "model=super-threaded\n"			\
  "super-threaded-scope=channel\n"		\
  "lock-global=ags-thread\n"			\
  "lock-parent=ags-recycling-thread\n"		\
  "\n"						\
  "[recall]\n"					\
  "auto-sense=true\n"				\
  "\n"


AgsAudioApplicationContext *audio_application_context;
gboolean midiin_test_finalized;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_midiin_test_init_suite()
{
  AgsConfig *config;

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_MIDIIN_TEST_CONFIG,
			    strlen(AGS_MIDIIN_TEST_CONFIG));
  
  audio_application_context = ags_audio_application_context_new();
  g_object_ref(audio_application_context);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_midiin_test_clean_suite()
{
  g_object_unref(audio_application_context);
  
  return(0);
}

void
ags_midiin_test_dispose()
{
  AgsMidiin *midiin;
  AgsAudio *audio;
  
  GList *list, *list_start;
  
  guint i;
  gboolean success;
  
  midiin = g_object_new(AGS_TYPE_MIDIIN,
			"application-context", audio_application_context,
			NULL);
  g_object_ref(midiin);
  
  /* instantiate audio list */
  list_start = NULL;
  
  for(i = 0; i < AGS_MIDIIN_TEST_DISPOSE_AUDIO_COUNT; i++){
    audio = g_object_new(AGS_TYPE_AUDIO,
			 NULL);
    g_object_ref(audio);    

    list_start = g_list_prepend(list_start,
				audio);
  }

  /* add to sequencer */
  list = list_start;
  
  while(list != NULL){
    /* audio list of sequencer */
    ags_sequencer_set_audio(AGS_SEQUENCER(midiin),
			    g_list_prepend(ags_sequencer_get_audio(AGS_SEQUENCER(midiin)),
					   list->data));
    g_object_ref(list->data);

    /* sequencer property of audio */
    g_object_set(list->data,
		 "sequencer", midiin,
		 NULL);

    /* iterate */
    list = list->next;
  }

  /* run dispose */
  g_object_run_dispose(midiin);

  /* assert no application context */
  CU_ASSERT(ags_sequencer_get_application_context(AGS_SEQUENCER(midiin)) == NULL);
  
  /* assert no audio */
  CU_ASSERT(ags_sequencer_get_audio(AGS_SEQUENCER(midiin)) == NULL);

  /* verify sequencer equals NULL */
  list = list_start;
  success = TRUE;
  
  while(list != NULL){
    GObject *sequencer;
    
    g_object_get(list->data,
		 "sequencer", &sequencer,
		 NULL);

    if(sequencer != NULL){
      success = FALSE;

      break;
    }
    
    list = list->next;
  }

  /* assert */
  CU_ASSERT(success == TRUE);
}

void
ags_midiin_test_finalize()
{
  AgsMidiin *midiin;
  AgsAudio *audio;
    
  guint i;
  
  midiin = g_object_new(AGS_TYPE_MIDIIN,
			"application-context", audio_application_context,
			NULL);

  /* audio list */  
  for(i = 0; i < AGS_MIDIIN_TEST_FINALIZE_AUDIO_COUNT; i++){
    /* instantiate audio */
    audio = g_object_new(AGS_TYPE_AUDIO,
			 NULL);
    g_object_ref(audio);    

    /* audio list of sequencer */
    ags_sequencer_set_audio(AGS_SEQUENCER(midiin),
			    g_list_prepend(ags_sequencer_get_audio(AGS_SEQUENCER(midiin)),
					   audio));
    g_object_ref(audio);

    /* sequencer property of audio */
    g_object_set(audio,
		 "sequencer", midiin,
		 NULL);
  }

  /* run dispose */
  g_object_run_dispose(midiin);

  /* stub finalize */
  midiin_test_finalized = FALSE;
  G_OBJECT_GET_CLASS(midiin)->finalize = ags_midiin_test_finalize_stub;

  /* unref and assert */
  g_object_unref(midiin);
  
  CU_ASSERT(midiin_test_finalized == TRUE);
}

void
ags_midiin_test_finalize_stub(GObject *gobject)
{
  midiin_test_finalized = TRUE;
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
  pSuite = CU_add_suite("AgsMidiinTest", ags_midiin_test_init_suite, ags_midiin_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsMidiin doing dispose", ags_midiin_test_dispose) == NULL) ||
     (CU_add_test(pSuite, "test of AgsMidiin doing finalize", ags_midiin_test_finalize) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
