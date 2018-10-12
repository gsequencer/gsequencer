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

int ags_apply_synth_test_init_suite();
int ags_apply_synth_test_clean_suite();

void ags_apply_synth_test_launch();

#define AGS_APPLY_SYNTH_TEST_BASE_NOTE (0.0)
#define AGS_APPLY_SYNTH_TEST_INPUT_PADS (88)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_apply_synth_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_apply_synth_test_clean_suite()
{  
  return(0);
}

void
ags_apply_synth_test_launch()
{
  AgsDevout *devout;
  AgsAudio *audio;
  AgsSynthGenerator *synth_generator;
  
  AgsApplySynth *apply_synth;
  
  AgsApplicationContext *application_context;
  
  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);

  devout = ags_devout_new(application_context);
  ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				   g_list_append(NULL, devout));
  g_object_ref(devout);

  audio = ags_audio_new(devout);
  ags_audio_set_flags(audio,
		      (AGS_AUDIO_SYNC |
		       AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		       AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(application_context),
			       g_list_append(NULL, audio));
  g_object_ref(audio);

  ags_audio_set_audio_channels(audio,
			       1, 0);

  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     AGS_APPLY_SYNTH_TEST_INPUT_PADS, 0);

  synth_generator = ags_synth_generator_new();

  apply_synth = ags_apply_synth_new(synth_generator,
				    audio->input,
				    AGS_APPLY_SYNTH_TEST_BASE_NOTE, AGS_APPLY_SYNTH_TEST_INPUT_PADS);

  CU_ASSERT(AGS_IS_APPLY_SYNTH(apply_synth));
  CU_ASSERT(apply_synth->synth_generator == synth_generator);
  CU_ASSERT(apply_synth->start_channel == audio->input);
  CU_ASSERT(apply_synth->count == AGS_APPLY_SYNTH_TEST_INPUT_PADS);

  /* test */
  ags_task_launch(apply_synth);
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
  pSuite = CU_add_suite("AgsApplySynthTest", ags_apply_synth_test_init_suite, ags_apply_synth_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsApplySynth launch", ags_apply_synth_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
