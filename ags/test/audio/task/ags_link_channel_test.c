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

int ags_link_channel_test_init_suite();
int ags_link_channel_test_clean_suite();

void ags_link_channel_test_launch();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_link_channel_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_link_channel_test_clean_suite()
{  
  return(0);
}

void
ags_link_channel_test_launch()
{
  AgsAudio *master_audio, *slave_audio;
  
  AgsLinkChannel *link_channel;

  guint i;

  /* master audio */
  master_audio = ags_audio_new(NULL);
  g_object_ref(master_audio);

  ags_audio_set_flags(master_audio,
		      (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		       AGS_AUDIO_INPUT_HAS_RECYCLING));

  /* create input/output */
  ags_audio_set_audio_channels(master_audio,
			       1, 0);

  ags_audio_set_pads(master_audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(master_audio,
		     AGS_TYPE_INPUT,
 		     1, 0);
  
  ags_connectable_connect(AGS_CONNECTABLE(master_audio));

  g_object_set(master_audio->input,
	       "file-link", ags_audio_file_link_new(),
	       NULL);
  
  /* slave audio */
  slave_audio = ags_audio_new(NULL);
  g_object_ref(slave_audio);

  ags_audio_set_flags(slave_audio,
		      (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		       AGS_AUDIO_INPUT_HAS_RECYCLING));

  /* create input/output */
  ags_audio_set_audio_channels(slave_audio,
			       1, 0);

  ags_audio_set_pads(slave_audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(slave_audio,
		     AGS_TYPE_INPUT,
 		     1, 0);
  
  ags_connectable_connect(AGS_CONNECTABLE(slave_audio));

  
  link_channel = ags_link_channel_new(master_audio->input,
				      slave_audio->output);

  CU_ASSERT(AGS_IS_LINK_CHANNEL(link_channel));
  CU_ASSERT(link_channel->channel == master_audio->input);
  CU_ASSERT(link_channel->link == slave_audio->output);

  /* launch */
  ags_task_launch(link_channel);

  CU_ASSERT(AGS_INPUT(master_audio->input)->file_link == NULL);
  CU_ASSERT(master_audio->input->link == slave_audio->output);
  CU_ASSERT(slave_audio->output->link == master_audio->input);
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
  pSuite = CU_add_suite("AgsLinkChannelTest", ags_link_channel_test_init_suite, ags_link_channel_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsLinkChannel launch", ags_link_channel_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
