/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2017 Joël Krähemann
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

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

int ags_recall_container_test_init_suite();
int ags_recall_container_test_clean_suite();

void ags_recall_container_test_get_recall_audio();
void ags_recall_container_test_get_recall_audio_run();
void ags_recall_container_test_get_recall_channel();
void ags_recall_container_test_get_recall_channel_run();
void ags_recall_container_test_find();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_container_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recall_container_test_clean_suite()
{  
  return(0);
}

void
ags_recall_container_test_get_recall_audio()
{
  //TODO:JK: implement me
}

void
ags_recall_container_test_get_recall_audio_run()
{
  //TODO:JK: implement me
}

void
ags_recall_container_test_get_recall_channel()
{
  //TODO:JK: implement me
}

void
ags_recall_container_test_get_recall_channel_run()
{
  //TODO:JK: implement me
}

void
ags_recall_container_test_find()
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
  pSuite = CU_add_suite("AgsRecallContainerTest", ags_recall_container_test_init_suite, ags_recall_container_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsRecallContainer get recall audio", ags_recall_container_test_get_recall_audio) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecallContainer get recall audio run", ags_recall_container_test_get_recall_audio_run) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecallContainer get recall channel", ags_recall_container_test_get_recall_channel) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecallContainer get recall channel run", ags_recall_container_test_get_recall_channel_run) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecallContainer find", ags_recall_container_test_find) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

