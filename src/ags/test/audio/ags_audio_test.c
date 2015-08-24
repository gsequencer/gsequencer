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
#include <CUnit/Basic.h>

#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

int ags_audio_test_init_suite();
int ags_audio_test_clean_suite();

void ags_audio_test_set_pads();
void ags_audio_test_set_audio_channels();
void ags_audio_test_link_channel();
void ags_audio_test_finalize_linked_channel();

#define AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS (2)
#define AGS_AUDIO_TEST_SET_PADS_INPUT_PADS (12)
#define AGS_AUDIO_TEST_SET_PADS_OUTPUT_PADS (5)

#define AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS (3)
#define AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_INPUT_PADS (1)
#define AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_OUTPUT_PADS (7)

AgsDevout *devout;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_audio_test_init_suite()
{
  devout = ags_devout_new(NULL);

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_audio_test_clean_suite()
{
  g_object_unref(devout);

  return(0);
}

void
ags_audio_test_set_pads()
{
  AgsAudio *audio;
  AgsChannel *channel, *current;

  guint i, j;
  
  /* instantiate */
  audio = ags_audio_new(AGS_SOUNDCARD(devout));

  CU_ASSERT(audio != NULL);

  /* set audio channels */
  audio->audio_channels = AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS;

  /* set input pads */
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_SET_PADS_INPUT_PADS);

  CU_ASSERT(audio->input_pads == AGS_AUDIO_TEST_SET_PADS_INPUT_PADS);

  /* verify pad count */
  channel = audio->input;
  
  for(i = 0; channel != audio->input->next_pad; i++){
    current = channel;
    
    for(j = 0; current != NULL; j++){
      current = current->next_pad;
    }
    
    CU_ASSERT(j == AGS_AUDIO_TEST_SET_PADS_INPUT_PADS);
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);

  /* verify line count */
  channel = audio->input;
  
  for(i = 0; channel != NULL; i++){
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_INPUT_PADS * AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);

  /* set output pads */
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_SET_PADS_OUTPUT_PADS);

  CU_ASSERT(audio->output_pads == AGS_AUDIO_TEST_SET_PADS_OUTPUT_PADS);

  /* verify pad count */
  channel = audio->output;
  
  for(i = 0; channel != audio->output->next_pad; i++){
    current = channel;
    
    for(j = 0; current != NULL; j++){
      current = current->next_pad;
    }
    
    CU_ASSERT(j == AGS_AUDIO_TEST_SET_PADS_OUTPUT_PADS);
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);

  /* verify line count */
  channel = audio->output;
  
  for(i = 0; channel != NULL; i++){
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_OUTPUT_PADS * AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);
}

void
ags_audio_test_set_audio_channels()
{
  AgsAudio *audio;
  AgsChannel *channel, *current;

  guint i, j;
  
  /* instantiate */
  audio = ags_audio_new(AGS_SOUNDCARD(devout));

  CU_ASSERT(audio != NULL);

  /* set pads */
  audio->output_pads = AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_OUTPUT_PADS;
  audio->input_pads = AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_INPUT_PADS;

  /* set audio channels */
  ags_audio_set_audio_channels(audio,
			       AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS);

  CU_ASSERT(audio->audio_channels == AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS);

  /* verify pad count */
  channel = audio->input;
  
  for(i = 0; channel != audio->input->next_pad; i++){
    current = channel;
    
    for(j = 0; current != NULL; j++){
      current = current->next_pad;
    }

    g_message("%d %d\0", j, AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_INPUT_PADS);
    CU_ASSERT(j == AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_INPUT_PADS);
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS);

  /* verify line count */
  channel = audio->input;
  
  for(i = 0; channel != NULL; i++){
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_INPUT_PADS * AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS);

  /* verify pad count */
  channel = audio->output;
  
  for(i = 0; channel != audio->output->next_pad; i++){
    current = channel;
    
    for(j = 0; current != NULL; j++){
      current = current->next_pad;
    }
    
    g_message("%d %d\0", j, AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_INPUT_PADS);
    CU_ASSERT(j == AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_OUTPUT_PADS);
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS);

  /* verify line count */
  channel = audio->output;
  
  for(i = 0; channel != NULL; i++){
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_OUTPUT_PADS * AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS);
}

void
ags_audio_test_link_channel()
{
  //TODO:JK: implement me
}

void
ags_audio_test_finalize_linked_channel()
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
  pSuite = CU_add_suite("AgsAudioTest\0", ags_audio_test_init_suite, ags_audio_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsAudio set pads\0", ags_audio_test_set_pads) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio set audio channels\0", ags_audio_test_set_audio_channels) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio link channel\0", ags_audio_test_link_channel) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio finalize linked channel\0", ags_audio_test_finalize_linked_channel) == NULL)){
      CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

