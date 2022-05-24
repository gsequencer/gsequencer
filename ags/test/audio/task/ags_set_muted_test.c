/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

int ags_set_muted_test_init_suite();
int ags_set_muted_test_clean_suite();

void ags_set_muted_test_launch();

AgsConfig *config;

AgsApplicationContext *application_context;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_set_muted_test_init_suite()
{
  ags_priority_load_defaults(ags_priority_get_instance());  

  config = ags_config_get_instance();
  
  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);

  ags_application_context_prepare(application_context);

  ags_application_context_setup(application_context);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_set_muted_test_clean_suite()
{  
  return(0);
}

void
ags_set_muted_test_launch()
{
  AgsAudio *audio;
  AgsChannel *channel;

  AgsSetMuted *set_muted;

  guint muted;
  gboolean success;
  
  audio = ags_audio_new(NULL);
  ags_audio_set_audio_channels(audio,
			       2, 0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     8, 0);

  /* ags-mute */
  ags_fx_factory_create(audio,
			ags_recall_container_new(), ags_recall_container_new(),
			"ags-fx-volume",
			NULL,
			NULL,
			0, 2,
			0, 8,
			0,
			(AGS_FX_FACTORY_INPUT,
			 AGS_FX_FACTORY_PLAY |
			 AGS_FX_FACTORY_RECALL |
			 AGS_FX_FACTORY_ADD),
			0);
  
  set_muted = ags_set_muted_new(audio,
				TRUE);
  
  CU_ASSERT(AGS_IS_SET_MUTED(set_muted));
  CU_ASSERT(set_muted->scope == audio);
  CU_ASSERT(set_muted->muted == TRUE);

  /* launch */
  ags_task_launch(set_muted);

  /* assert */
  success = TRUE;
  
  channel = audio->input;

  while(channel != NULL){
    GList *start_play, *play;
    GList *start_recall, *recall;

    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,		 
		 NULL);

    play = ags_recall_template_find_type(start_play,
					 AGS_TYPE_FX_VOLUME_CHANNEL);
    recall = ags_recall_template_find_type(start_recall,
					   AGS_TYPE_FX_VOLUME_CHANNEL);


    if(!AGS_FX_VOLUME_CHANNEL(play->data)->muted->port_value.ags_port_boolean ||
       !AGS_FX_VOLUME_CHANNEL(recall->data)->muted->port_value.ags_port_boolean){
      success = FALSE;

      break;
    }
    
    channel = channel->next;
  }

  CU_ASSERT(success == TRUE);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C");
  putenv("LANG=C");

  putenv("LADSPA_PATH=\"\"");
  putenv("DSSI_PATH=\"\"");
  putenv("LV2_PATH=\"\"");
    
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsSetMutedTest", ags_set_muted_test_init_suite, ags_set_muted_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsSetMuted launch", ags_set_muted_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
