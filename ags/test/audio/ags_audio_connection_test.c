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

int ags_audio_connection_test_init_suite();
int ags_audio_connection_test_clean_suite();

void ags_audio_connection_test_find();

#define AGS_AUDIO_CONNECTION_TEST_FIND_PAD_0 (0)
#define AGS_AUDIO_CONNECTION_TEST_FIND_AUDIO_CHANNEL_0 (0)
#define AGS_AUDIO_CONNECTION_TEST_FIND_PAD_1 (3)
#define AGS_AUDIO_CONNECTION_TEST_FIND_AUDIO_CHANNEL_1 (9)
#define AGS_AUDIO_CONNECTION_TEST_FIND_PAD_2 (2)
#define AGS_AUDIO_CONNECTION_TEST_FIND_AUDIO_CHANNEL_2 (7)


/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_audio_connection_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_audio_connection_test_clean_suite()
{
  return(0);
}

void
ags_audio_connection_test_find()
{
  AgsAudioConnection *audio_connection[3];

  GList *list, *current;

  list = NULL;
  
  audio_connection[0] = g_object_new(AGS_TYPE_AUDIO_CONNECTION,
				     "channel-type", AGS_TYPE_INPUT,
				     "pad", AGS_AUDIO_CONNECTION_TEST_FIND_PAD_0,
				     "audio-channel", AGS_AUDIO_CONNECTION_TEST_FIND_AUDIO_CHANNEL_0,
				     NULL);
  list = g_list_prepend(list,
			audio_connection[0]);

  
  audio_connection[1] = g_object_new(AGS_TYPE_AUDIO_CONNECTION,
				     "channel-type", AGS_TYPE_INPUT,
				     "pad", AGS_AUDIO_CONNECTION_TEST_FIND_PAD_1,
				     "audio-channel", AGS_AUDIO_CONNECTION_TEST_FIND_AUDIO_CHANNEL_1,
				     NULL);
  list = g_list_prepend(list,
			audio_connection[1]);

  
  audio_connection[2] = g_object_new(AGS_TYPE_AUDIO_CONNECTION,
				     "channel-type", AGS_TYPE_INPUT,
				     "pad", AGS_AUDIO_CONNECTION_TEST_FIND_PAD_2,
				     "audio-channel", AGS_AUDIO_CONNECTION_TEST_FIND_AUDIO_CHANNEL_2,
				     NULL);
  list = g_list_prepend(list,
			audio_connection[2]);

  /* find and assert */
  CU_ASSERT((current = ags_audio_connection_find(list,
						 AGS_TYPE_INPUT,
						 AGS_AUDIO_CONNECTION_TEST_FIND_PAD_0,
						 AGS_AUDIO_CONNECTION_TEST_FIND_AUDIO_CHANNEL_0)) != NULL &&
	    current->data == audio_connection[0]);

  CU_ASSERT((current = ags_audio_connection_find(list,
						 AGS_TYPE_INPUT,
						 AGS_AUDIO_CONNECTION_TEST_FIND_PAD_1,
						 AGS_AUDIO_CONNECTION_TEST_FIND_AUDIO_CHANNEL_1)) != NULL &&
	    current->data == audio_connection[1]);

  CU_ASSERT((current = ags_audio_connection_find(list,
						 AGS_TYPE_INPUT,
						 AGS_AUDIO_CONNECTION_TEST_FIND_PAD_2,
						 AGS_AUDIO_CONNECTION_TEST_FIND_AUDIO_CHANNEL_2)) != NULL &&
	    current->data == audio_connection[2]);
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
  pSuite = CU_add_suite("AgsAudioConnectionTest", ags_audio_connection_test_init_suite, ags_audio_connection_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsAudioConnection find", ags_audio_connection_test_find) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
