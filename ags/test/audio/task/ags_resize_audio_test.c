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

int ags_resize_audio_test_init_suite();
int ags_resize_audio_test_clean_suite();

void ags_resize_audio_test_launch();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_resize_audio_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_resize_audio_test_clean_suite()
{  
  return(0);
}

void
ags_resize_audio_test_launch()
{
  AgsAudio *audio;

  AgsResizeAudio *resize_audio;

  audio = ags_audio_new(NULL);

  resize_audio = ags_resize_audio_new(audio,
				      1,
				      8,
				      2);
  
  CU_ASSERT(AGS_IS_RESIZE_AUDIO(resize_audio));
  CU_ASSERT(resize_audio->audio == audio);
  CU_ASSERT(resize_audio->output_pads == 1);
  CU_ASSERT(resize_audio->input_pads == 8);
  CU_ASSERT(resize_audio->audio_channels == 2);

  /* launch */
  ags_task_launch(resize_audio);

  CU_ASSERT(audio->output_pads == 1);
  CU_ASSERT(audio->input_pads == 8);
  CU_ASSERT(audio->audio_channels == 2);
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
  pSuite = CU_add_suite("AgsResizeAudioTest", ags_resize_audio_test_init_suite, ags_resize_audio_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsResizeAudio launch", ags_resize_audio_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
