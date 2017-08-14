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

int ags_input_test_init_suite();
int ags_input_test_clean_suite();

void ags_input_test_open_file();
void ags_input_test_apply_synth();
void ags_input_test_apply_synth_extended();
void ags_input_test_is_active();
void ags_input_test_next_active();

#define AGS_INPUT_TEST_OPEN_FILE_FILENAME "/usr/share/sounds/alsa/Noise.wav"
#define AGS_INPUT_TEST_OPEN_FILE_AUDIO_CHANNEL (0)

#define AGS_INPUT_TEST_APPLY_SYNTH_OSCILLATOR (AGS_SYNTH_GENERATOR_OSCILLATOR_SIN)
#define AGS_INPUT_TEST_APPLY_SYNTH_FREQUENCY (440.0)
#define AGS_INPUT_TEST_APPLY_SYNTH_PHASE (0.0)
#define AGS_INPUT_TEST_APPLY_SYNTH_VOLUME (0.5)
#define AGS_INPUT_TEST_APPLY_SYNTH_N_FRAMES (3200)

#define AGS_INPUT_TEST_APPLY_SYNTH_EXTENDED_OSCILLATOR (AGS_SYNTH_GENERATOR_OSCILLATOR_SIN)
#define AGS_INPUT_TEST_APPLY_SYNTH_EXTENDED_FREQUENCY (440.0)
#define AGS_INPUT_TEST_APPLY_SYNTH_EXTENDED_PHASE (0.0)
#define AGS_INPUT_TEST_APPLY_SYNTH_EXTENDED_VOLUME (0.5)
#define AGS_INPUT_TEST_APPLY_SYNTH_EXTENDED_N_FRAMES (3200)
#define AGS_INPUT_TEST_APPLY_SYNTH_EXTENDED_ATTACK (0)
#define AGS_INPUT_TEST_APPLY_SYNTH_EXTENDED_BASE_NOTE (-48.0)
#define AGS_INPUT_TEST_APPLY_SYNTH_EXTENDED_COMPUTE_FLAGS (AGS_SYNTH_GENERATOR_COMPUTE_FIXED_LENGTH)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_input_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_input_test_clean_suite()
{
  return(0);
}

void
ags_input_test_open_file()
{
  AgsInput *input;

  gboolean success;
  
  input = g_object_new(AGS_TYPE_INPUT,
		       NULL);
  AGS_CHANNEL(input)->first_recycling =
    AGS_CHANNEL(input)->last_recycling = g_object_new(AGS_TYPE_RECYCLING,
						      NULL);
  
  success = ags_input_open_file(input,
				AGS_INPUT_TEST_OPEN_FILE_FILENAME,
				NULL,
				NULL,
				NULL,
				AGS_INPUT_TEST_OPEN_FILE_AUDIO_CHANNEL);
  
  CU_ASSERT(success == TRUE &&
	    input->file_link != NULL &&
	    AGS_CHANNEL(input)->first_recycling->audio_signal != NULL);
}

void
ags_input_test_apply_synth()
{
  AgsInput *input;

  gboolean success;
  
  input = g_object_new(AGS_TYPE_INPUT,
		       NULL);
  AGS_CHANNEL(input)->first_recycling =
    AGS_CHANNEL(input)->last_recycling = g_object_new(AGS_TYPE_RECYCLING,
						      NULL);
  
  success = ags_input_apply_synth(input,
				  AGS_INPUT_TEST_APPLY_SYNTH_OSCILLATOR,
				  AGS_INPUT_TEST_APPLY_SYNTH_FREQUENCY,
				  AGS_INPUT_TEST_APPLY_SYNTH_PHASE,
				  AGS_INPUT_TEST_APPLY_SYNTH_VOLUME,
				  AGS_INPUT_TEST_APPLY_SYNTH_N_FRAMES);
  
  CU_ASSERT(success == TRUE &&
	    input->file_link != NULL &&
	    AGS_CHANNEL(input)->first_recycling->audio_signal != NULL);
}

void
ags_input_test_apply_synth_extended()
{
  AgsInput *input;

  gboolean success;
  
  input = g_object_new(AGS_TYPE_INPUT,
		       NULL);
  AGS_CHANNEL(input)->first_recycling =
    AGS_CHANNEL(input)->last_recycling = g_object_new(AGS_TYPE_RECYCLING,
						      NULL);
  
  success = ags_input_apply_synth_extended(input,
					   AGS_INPUT_TEST_APPLY_SYNTH_EXTENDED_OSCILLATOR,
					   AGS_INPUT_TEST_APPLY_SYNTH_EXTENDED_FREQUENCY,
					   AGS_INPUT_TEST_APPLY_SYNTH_EXTENDED_PHASE,
					   AGS_INPUT_TEST_APPLY_SYNTH_EXTENDED_VOLUME,
					   AGS_INPUT_TEST_APPLY_SYNTH_EXTENDED_N_FRAMES,
					   AGS_INPUT_TEST_APPLY_SYNTH_EXTENDED_ATTACK,
					   AGS_INPUT_TEST_APPLY_SYNTH_EXTENDED_BASE_NOTE,
					   NULL, NULL,
					   AGS_INPUT_TEST_APPLY_SYNTH_EXTENDED_COMPUTE_FLAGS);
  
  CU_ASSERT(success == TRUE &&
	    input->file_link != NULL &&
	    AGS_CHANNEL(input)->first_recycling->audio_signal != NULL);
}

void
ags_input_test_is_active()
{
  AgsInput *input;
  AgsAudioSignal *audio_signal;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context, *parent_recycling_context;

  gboolean is_active;
  
  input = g_object_new(AGS_TYPE_INPUT,
		       NULL);
  AGS_CHANNEL(input)->first_recycling =
    AGS_CHANNEL(input)->last_recycling = g_object_new(AGS_TYPE_RECYCLING,
						      NULL);

  recycling_context = g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
				   NULL);
  
  parent_recycling_context = g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
					  "parent", recycling_context,
					  NULL);
  
  /* assert - not active */
  is_active = ags_input_is_active(input,
				  recycling_context);

  CU_ASSERT(is_active == FALSE);
  
  /* assert - active */
  recall_id = g_object_new(AGS_TYPE_RECALL_ID,
			   "recycling-context", recycling_context,
			   NULL);
  
  audio_signal = g_object_new(AGS_TYPE_AUDIO_SIGNAL,
			      "recall-id", recall_id,
			      NULL);
  ags_recycling_add_audio_signal(AGS_CHANNEL(input)->first_recycling,
				 audio_signal);
  
  is_active = ags_input_is_active(input,
				  recycling_context);

  CU_ASSERT(is_active == TRUE);
}

void
ags_input_test_next_active()
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
  pSuite = CU_add_suite("AgsInputTest", ags_input_test_init_suite, ags_input_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  //  g_log_set_fatal_mask("GLib-GObject", // "Gtk" G_LOG_DOMAIN,
  //		       G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING);

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsInput open file", ags_input_test_open_file) == NULL) ||
     (CU_add_test(pSuite, "test of AgsInput apply synth", ags_input_test_apply_synth) == NULL) ||
     (CU_add_test(pSuite, "test of AgsInput apply synth extended", ags_input_test_apply_synth_extended) == NULL) ||
     (CU_add_test(pSuite, "test of AgsInput is active", ags_input_test_is_active) == NULL) ||
     (CU_add_test(pSuite, "test of AgsInput next active", ags_input_test_next_active) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

