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

int ags_output_test_init_suite();
int ags_output_test_clean_suite();

void ags_output_test_find_first_input_recycling();
void ags_output_test_find_last_input_recycling();

#define AGS_OUTPUT_TEST_FIND_FIRST_INPUT_RECYCLING_AUDIO_CHANNELS (2)
#define AGS_OUTPUT_TEST_FIND_FIRST_INPUT_RECYCLING_OUTPUT_PADS (1)
#define AGS_OUTPUT_TEST_FIND_FIRST_INPUT_RECYCLING_INPUT_PADS (8)

#define AGS_OUTPUT_TEST_FIND_LAST_INPUT_RECYCLING_AUDIO_CHANNELS (2)
#define AGS_OUTPUT_TEST_FIND_LAST_INPUT_RECYCLING_OUTPUT_PADS (1)
#define AGS_OUTPUT_TEST_FIND_LAST_INPUT_RECYCLING_INPUT_PADS (8)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_output_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_output_test_clean_suite()
{
  return(0);
}

void
ags_output_test_find_first_input_recycling()
{
  AgsAudio *audio;

  AgsChannel *current;
  
  AgsRecycling *recycling, *first_recycling;

  guint i;
  
  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  ags_audio_set_audio_channels(audio,
			       AGS_OUTPUT_TEST_FIND_FIRST_INPUT_RECYCLING_AUDIO_CHANNELS);

  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     AGS_OUTPUT_TEST_FIND_FIRST_INPUT_RECYCLING_OUTPUT_PADS);
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     AGS_OUTPUT_TEST_FIND_FIRST_INPUT_RECYCLING_INPUT_PADS);

  /* create some recycling */
  current = ags_channel_pad_nth(audio->input,
				1);
  first_recycling = current->first_recycling;
  
  for(i = 0; i < 2; i++){
    current->first_recycling =
      current->last_recycling = g_object_new(AGS_TYPE_RECYCLING,
					     NULL);
    
    current = current->next_pad;
  }

  /* create more recycling */
  current = ags_channel_pad_nth(audio->input,
				5);
  
  for(i = 0; i < 2; i++){
    current->first_recycling =
      current->last_recycling= g_object_new(AGS_TYPE_RECYCLING,
					    NULL);
    
    current = current->next_pad;
  }

  /* assert */
  recycling = ags_output_find_first_input_recycling(audio->output);

  CU_ASSERT(recycling == first_recycling);
}

void
ags_output_test_find_last_input_recycling()
{
  AgsAudio *audio;

  AgsChannel *current;
  
  AgsRecycling *recycling, *last_recycling;

  guint i;

  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);
  
  ags_audio_set_audio_channels(audio,
			       AGS_OUTPUT_TEST_FIND_LAST_INPUT_RECYCLING_AUDIO_CHANNELS);

  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     AGS_OUTPUT_TEST_FIND_LAST_INPUT_RECYCLING_OUTPUT_PADS);
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     AGS_OUTPUT_TEST_FIND_LAST_INPUT_RECYCLING_INPUT_PADS);

  /* create some recycling */
  current = ags_channel_pad_nth(audio->input,
				1);
  
  for(i = 0; i < 2; i++){
    current->first_recycling =
      current->last_recycling = g_object_new(AGS_TYPE_RECYCLING,
					     NULL);
    
    current = current->next_pad;
  }

  /* create more recycling */
  current = ags_channel_pad_nth(audio->input,
				5);
  
  for(i = 0; i < 2; i++){
    current->first_recycling =
      current->last_recycling = g_object_new(AGS_TYPE_RECYCLING,
					     NULL);
    
    current = current->next_pad;
  }

  current = ags_channel_pad_nth(audio->input,
				7);
  last_recycling = current->last_recycling;

  /* assert */
  recycling = ags_output_find_last_input_recycling(audio->output);

  CU_ASSERT(recycling == last_recycling);
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
  pSuite = CU_add_suite("AgsOutputTest", ags_output_test_init_suite, ags_output_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsOutput find first input recycling", ags_output_test_find_first_input_recycling) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOutput find last input recycling", ags_output_test_find_last_input_recycling) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

