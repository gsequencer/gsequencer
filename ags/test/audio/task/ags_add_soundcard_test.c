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

int ags_add_soundcard_test_init_suite();
int ags_add_soundcard_test_clean_suite();

void ags_add_soundcard_test_launch();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_add_soundcard_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_add_soundcard_test_clean_suite()
{  
  return(0);
}

void
ags_add_soundcard_test_launch()
{
  AgsAlsaDevout *devout;
  
  AgsAddSoundcard *add_soundcard;

  AgsApplicationContext *application_context;

  GList *list;
  
  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);

  devout = ags_alsa_devout_new(application_context);
  g_object_ref(devout);
  
  add_soundcard = ags_add_soundcard_new(devout);

  CU_ASSERT(AGS_IS_ADD_SOUNDCARD(add_soundcard));
  CU_ASSERT(add_soundcard->soundcard == devout);

  /* test */
  ags_task_launch(add_soundcard);

  list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

  CU_ASSERT(list != NULL);
  CU_ASSERT(list->data == devout);
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
  pSuite = CU_add_suite("AgsAddSoundcardTest", ags_add_soundcard_test_init_suite, ags_add_soundcard_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsAddSoundcard launch", ags_add_soundcard_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
