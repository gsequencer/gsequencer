/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2018 Joël Krähemann
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

#include <stdlib.h>

int ags_synth_generator_test_init_suite();
int ags_synth_generator_test_clean_suite();

void ags_synth_generator_test_compute();

#define AGS_SYNTH_GENERATOR_TEST_AUDIO_SIGNAL_LENGTH (24)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_synth_generator_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_synth_generator_test_clean_suite()
{  
  return(0);
}

void
ags_synth_generator_test_compute()
{
  AgsAudioSignal *audio_signal;
  AgsSynthGenerator *synth_generator;

  GList *list;
  
  guint xcross_count;
  
  audio_signal = ags_audio_signal_new(NULL,
				      NULL,
				      NULL);
  ags_audio_signal_stream_resize(audio_signal,
				 AGS_SYNTH_GENERATOR_TEST_AUDIO_SIGNAL_LENGTH);
  
  synth_generator = ags_synth_generator_new();
  
  /* test - note 0.0 */
  synth_generator->oscillator = AGS_SYNTH_OSCILLATOR_SIN;

  synth_generator->frequency = 440.0;
  synth_generator->volume = 0.5;
  
  synth_generator->frame_count = AGS_SYNTH_GENERATOR_TEST_AUDIO_SIGNAL_LENGTH * audio_signal->buffer_size;

  ags_synth_generator_compute(synth_generator,
			      audio_signal,
			      0.0);

  list = audio_signal->stream;
  xcross_count = 0;
  
  while(list != NULL){
    xcross_count += ags_synth_util_get_xcross_count(list->data,
						    ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
						    audio_signal->buffer_size);

    list = list->next;
  }

  CU_ASSERT(xcross_count > 0);

  /* test - note 48.0 */
  synth_generator->oscillator = AGS_SYNTH_OSCILLATOR_SIN;

  synth_generator->frequency = 440.0;
  synth_generator->volume = 0.5;
  
  synth_generator->frame_count = AGS_SYNTH_GENERATOR_TEST_AUDIO_SIGNAL_LENGTH * audio_signal->buffer_size;

  ags_synth_generator_compute(synth_generator,
			      audio_signal,
			      48.0);

  list = audio_signal->stream;
  xcross_count = 0;
  
  while(list != NULL){
    xcross_count += ags_synth_util_get_xcross_count(list->data,
						    ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
						    audio_signal->buffer_size);

    list = list->next;
  }

  CU_ASSERT(xcross_count > 0);
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
  pSuite = CU_add_suite("AgsSynthGeneratorTest", ags_synth_generator_test_init_suite, ags_synth_generator_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsSynthGenerator compute", ags_synth_generator_test_compute) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
