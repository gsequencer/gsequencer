/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

int ags_remove_audio_test_init_suite();
int ags_remove_audio_test_clean_suite();

void ags_remove_audio_test_launch();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_remove_audio_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_remove_audio_test_clean_suite()
{  
  return(0);
}

void
ags_remove_audio_test_launch()
{
  AgsAlsaDevout *devout;
  AgsAudio *audio;

  AgsRemoveAudio *remove_audio;

  AgsApplicationContext *application_context;

  GList *list;
  
  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);

  /* create soundcard */
  devout = g_object_new(AGS_TYPE_ALSA_DEVOUT,
			NULL);
  g_object_ref(devout);

  /* create audio */
  audio = ags_audio_new(devout);
  g_object_ref(audio);

  list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));
  list = g_list_append(list,
		       audio);
  g_object_ref(audio);
  ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(application_context),
			       list);

  /* create remove audio */
  remove_audio = ags_remove_audio_new(audio);

  CU_ASSERT(AGS_IS_REMOVE_AUDIO(remove_audio));
  CU_ASSERT(remove_audio->audio == audio);

  /* launch */
  ags_task_launch(remove_audio);

  list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

  CU_ASSERT(g_list_find(list, audio) == NULL);
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
  pSuite = CU_add_suite("AgsRemoveAudioTest", ags_remove_audio_test_init_suite, ags_remove_audio_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* remove the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsRemoveAudio launch", ags_remove_audio_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
