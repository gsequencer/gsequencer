/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>

int ags_recycling_test_init_suite();
int ags_recycling_test_clean_suite();

void ags_recycling_test_add_audio_signal();
void ags_recycling_test_remove_audio_signal();
void ags_recycling_test_create_audio_signal_with_defaults();
void ags_recycling_test_create_audio_signal_with_frame_count();
void ags_recycling_test_position();
void ags_recycling_test_find_next_channel();

AgsDevout *devout;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recycling_test_init_suite()
{ 
  devout = ags_devout_new(NULL);

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recycling_test_clean_suite()
{
  g_object_unref(devout);
  
  return(0);
}

void
ags_recycling_test_add_audio_signal()
{
  AgsRecycling *recycling;

  /*  */
  recycling = ags_recycling_new(NULL);
  //TODO:JK: implement me
}

void
ags_recycling_test_remove_audio_signal()
{
  //TODO:JK: implement me
}

void
ags_recycling_test_create_audio_signal_with_defaults()
{
  //TODO:JK: implement me
}

void
ags_recycling_test_create_audio_signal_with_frame_count()
{
  //TODO:JK: implement me
}

void
ags_recycling_test_position()
{
  //TODO:JK: implement me
}

void
ags_recycling_test_find_next_channel()
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
  pSuite = CU_add_suite("AgsRecyclingTest\0", ags_recycling_test_init_suite, ags_recycling_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsRecycling add audio signal\0", ags_recycling_test_add_audio_signal) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecycling remove audio signal\0", ags_recycling_test_remove_audio_signal) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecycling create audio signal with defaults\0", ags_recycling_test_create_audio_signal_with_defaults) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecycling create audio signal with frame count\0", ags_recycling_test_create_audio_signal_with_frame_count) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecycling position\0", ags_recycling_test_position) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecycling find next channel\0", ags_recycling_test_find_next_channel) == NULL)){
      CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

