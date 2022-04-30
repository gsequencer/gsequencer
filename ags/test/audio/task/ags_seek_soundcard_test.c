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

int ags_seek_soundcard_test_init_suite();
int ags_seek_soundcard_test_clean_suite();

void ags_seek_soundcard_test_launch();

AgsApplicationContext *application_context;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_seek_soundcard_test_init_suite()
{
  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_seek_soundcard_test_clean_suite()
{  
  return(0);
}

void
ags_seek_soundcard_test_launch()
{
  AgsAlsaDevout *devout;

  AgsSeekSoundcard *seek_soundcard;

  devout = ags_alsa_devout_new(NULL);

  seek_soundcard = ags_seek_soundcard_new(devout,
					  64 * 16,
					  AGS_SEEK_SET);
  
  CU_ASSERT(AGS_IS_SEEK_SOUNDCARD(seek_soundcard));
  CU_ASSERT(seek_soundcard->soundcard == devout);
  CU_ASSERT(seek_soundcard->offset == 64 * 16);
  CU_ASSERT(seek_soundcard->whence == AGS_SEEK_SET);

  /* launch */
  ags_task_launch(seek_soundcard);

  CU_ASSERT(ags_soundcard_get_note_offset(AGS_SOUNDCARD(devout)) == 64 * 16);
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
  pSuite = CU_add_suite("AgsSeekSoundcardTest", ags_seek_soundcard_test_init_suite, ags_seek_soundcard_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsSeekSoundcard launch", ags_seek_soundcard_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
