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

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recycling_context.h>

int ags_channel_test_init_suite();
int ags_channel_test_clean_suite();

void ags_channel_test_add_recall();
void ags_channel_test_add_recall_container();
void ags_channel_test_add_recall_id();
void ags_channel_test_add_recycling_context();
void ags_channel_test_duplicate_recall();
void ags_channel_test_init_recall();
void ags_channel_test_resolve_recall();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_channel_test_init_suite()
{ 
  //TODO:JK: implement me
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_channel_test_clean_suite()
{
  //TODO:JK: implement me
}

void
ags_channel_test_add_recall()
{
  //TODO:JK: implement me
}

void
ags_channel_test_add_recall_container()
{
  //TODO:JK: implement me
}

void
ags_channel_test_add_recall_id()
{
  //TODO:JK: implement me
}

void
ags_channel_test_add_recycling_context()
{
  //TODO:JK: implement me
}

void
ags_channel_test_duplicate_recall()
{
  //TODO:JK: implement me
}

void
ags_channel_test_init_recall()
{
  //TODO:JK: implement me
}

void
ags_channel_test_resolve_recall()
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
  pSuite = CU_add_suite("AgsChannelTest\0", ags_channel_test_init_suite, ags_channel_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsChannel add recall\0", ags_channel_test_add_recall) == NULL) ||
     (CU_add_test(pSuite, "test of AgsChannel add recall container\0", ags_channel_test_add_recall_container) == NULL) ||
     (CU_add_test(pSuite, "test of AgsChannel add recall id\0", ags_channel_test_add_recall_id) == NULL) ||
     (CU_add_test(pSuite, "test of AgsChannel add recycling context\0", ags_channel_test_add_recycling_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsChannel add duplicate recall\0", ags_channel_test_add_duplicate_recall) == NULL) ||
     (CU_add_test(pSuite, "test of AgsChannel add resolve recall\0", ags_channel_test_add_resolve_recall) == NULL) ||
     (CU_add_test(pSuite, "test of AgsChannel add init recall\0", ags_channel_test_add_init_recall) == NULL)){
      CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

