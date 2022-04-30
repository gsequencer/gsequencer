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

int ags_clear_audio_signal_test_init_suite();
int ags_clear_audio_signal_test_clean_suite();

void ags_clear_audio_signal_test_launch();

#define AGS_CLEAR_AUDIO_SIGNAL_TEST_STREAM_LENGTH (24)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_clear_audio_signal_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_clear_audio_signal_test_clean_suite()
{  
  return(0);
}

void
ags_clear_audio_signal_test_launch()
{
  AgsAlsaDevout *devout;
  AgsAudioSignal *audio_signal;

  AgsClearAudioSignal *clear_audio_signal;

  GList *list;
  
  guint i;
  gboolean success;
  
  /* create soundcard */
  devout = g_object_new(AGS_TYPE_ALSA_DEVOUT,
			NULL);
  g_object_ref(devout);

  /* create audio signal */
  audio_signal = ags_audio_signal_new(devout,
				      NULL,
				      NULL);
  g_object_set(audio_signal,
	       "format", AGS_SOUNDCARD_SIGNED_16_BIT,
	       NULL);
  g_object_ref(audio_signal);

  ags_audio_signal_stream_resize(audio_signal,
				 AGS_CLEAR_AUDIO_SIGNAL_TEST_STREAM_LENGTH);
  
  /* fill random data */
  list = audio_signal->stream;
  
  while(list != NULL){
    gint16 *buffer;
    
    buffer = list->data;

    for(i = 0; i < audio_signal->buffer_size; i++){
      buffer[i] = rand() % G_MAXINT16;
    }

    list = list->next;
  }
  
  /* create clear audio signal */
  clear_audio_signal = ags_clear_audio_signal_new(audio_signal);

  CU_ASSERT(AGS_IS_CLEAR_AUDIO_SIGNAL(clear_audio_signal));
  CU_ASSERT(clear_audio_signal->audio_signal == audio_signal);

  /* launch */
  ags_task_launch(clear_audio_signal);

  success = TRUE;

  list = audio_signal->stream;
  
  while(list != NULL){
    gint16 *buffer;
    
    buffer = list->data;

    for(i = 0; i < audio_signal->buffer_size; i++){
      if(buffer[i] != 0){
	success = FALSE;

	goto ags_clear_audio_signal_launch_LOOP_END;
      }
    }

    list = list->next;
  }

 ags_clear_audio_signal_launch_LOOP_END:
  CU_ASSERT(success == TRUE);
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
  pSuite = CU_add_suite("AgsClearAudioSignalTest", ags_clear_audio_signal_test_init_suite, ags_clear_audio_signal_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsClearAudioSignal launch", ags_clear_audio_signal_test_launch) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
