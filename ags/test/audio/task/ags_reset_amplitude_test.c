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

int ags_reset_amplitude_test_init_suite();
int ags_reset_amplitude_test_clean_suite();

void ags_reset_amplitude_test_launch();

AgsDevout *devout;
AgsAudio *audio;

AgsApplicationContext *application_context;

GList *recall;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_reset_amplitude_test_init_suite()
{ 
  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);

  ags_application_context_prepare(application_context);
  ags_application_context_setup(application_context);
  
  /* create soundcard */
  devout = g_object_new(AGS_TYPE_DEVOUT,
			NULL);
  g_object_ref(devout);

  /* create audio */
  audio = ags_audio_new(devout);
  g_object_ref(audio);

  ags_audio_set_flags(audio,
		      (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		       AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_ability_flags(audio,
			      AGS_SOUND_ABILITY_PLAYBACK);

  /* create input/output */
  ags_audio_set_audio_channels(audio,
			       1, 0);

  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
 		     1, 0);

  ags_channel_set_ability_flags(audio->output,
				AGS_SOUND_ABILITY_PLAYBACK);

  ags_channel_set_ability_flags(audio->input,
				AGS_SOUND_ABILITY_PLAYBACK);

  ags_connectable_connect(AGS_CONNECTABLE(audio));
  
  ags_connectable_connect(AGS_CONNECTABLE(audio->output));
  ags_connectable_connect(AGS_CONNECTABLE(audio->input));
  
  /* create ags-analyse */
  recall = ags_recall_factory_create(audio,
				     NULL, NULL,
				     "ags-analyse",
				     0, 1,
				     0, 1,
				     (AGS_RECALL_FACTORY_INPUT |
				      AGS_RECALL_FACTORY_ADD |
				      AGS_RECALL_FACTORY_PLAY |
				      AGS_RECALL_FACTORY_RECALL),
				     0);

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_reset_amplitude_test_clean_suite()
{  
  g_object_run_dispose(devout);
  g_object_unref(devout);

  g_object_run_dispose(audio);
  g_object_unref(audio);

  return(0);
}

void
ags_reset_amplitude_test_launch()
{
  AgsResetAmplitude *reset_amplitude;

  GList *list;
  
  GValue value = {0,};
  
  reset_amplitude = ags_reset_amplitude_get_instance();

  CU_ASSERT(AGS_IS_RESET_AMPLITUDE(reset_amplitude));

  list = ags_recall_find_type(audio->input->recall,
			      AGS_TYPE_ANALYSE_CHANNEL);
  
  ags_reset_amplitude_add(reset_amplitude,
			  list->data);

  g_value_init(&value,
	       G_TYPE_BOOLEAN);
  g_value_set_boolean(&value,
		      TRUE);

  ags_port_safe_write(AGS_ANALYSE_CHANNEL(list->data)->buffer_computed,
		      &value);

  /* test */
  ags_task_launch(reset_amplitude);

  ags_port_safe_read(AGS_ANALYSE_CHANNEL(list->data)->buffer_computed,
		     &value);

  CU_ASSERT(g_value_get_boolean(&value) == FALSE);
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
  pSuite = CU_add_suite("AgsResetAmplitudeTest", ags_reset_amplitude_test_init_suite, ags_reset_amplitude_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsResetAmplitude launch", ags_reset_amplitude_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
