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
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

int ags_audio_signal_test_init_suite();
int ags_audio_signal_test_clean_suite();

void ags_audio_signal_test_add_stream();
void ags_audio_signal_test_resize_stream();
void ags_audio_signal_test_realloc_buffer_size();
void ags_audio_signal_test_copy_buffer_to_buffer();
void ags_audio_signal_test_copy_double_buffer_to_buffer();
void ags_audio_signal_test_duplicate_stream();
void ags_audio_signal_test_get_template();
void ags_audio_signal_test_get_stream_current();
void ags_audio_signal_test_get_by_recall_id();
void ags_audio_signal_test_tile();
void ags_audio_signal_test_scale();

#define AGS_AUDIO_SIGNAL_TEST_GET_TEMPLATE_N_AUDIO_SIGNAL (3)
#define AGS_AUDIO_SIGNAL_TEST_GET_STREAM_CURRENT_N_AUDIO_SIGNAL (6)

AgsDevout *devout;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_audio_signal_test_init_suite()
{ 
  devout = ags_devout_new(NULL);
  g_object_ref(devout);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_audio_signal_test_clean_suite()
{
  g_object_unref(devout);

  return(0);
}

void
ags_audio_signal_test_add_stream()
{
  AgsAudioSignal *audio_signal;

  /* instantiate audio signal */
  audio_signal = ags_audio_signal_new(G_OBJECT(devout),
				      NULL,
				      NULL);

  /* assert add stream */
  CU_ASSERT(audio_signal->stream == NULL);

  ags_audio_signal_add_stream(audio_signal);
  CU_ASSERT(audio_signal->stream != NULL);
  CU_ASSERT(g_list_length(audio_signal->stream) == 1);
}

void
ags_audio_signal_test_resize_stream()
{
  AgsAudioSignal *audio_signal;

  /* instantiate audio signal */
  audio_signal = ags_audio_signal_new(G_OBJECT(devout),
				      NULL,
				      NULL);

  /* assert resize stream */
  CU_ASSERT(audio_signal->stream == NULL);

  ags_audio_signal_stream_resize(audio_signal,
				 5);
  CU_ASSERT(audio_signal->stream != NULL);
  CU_ASSERT(g_list_length(audio_signal->stream) == 5);
}

void
ags_audio_signal_test_realloc_buffer_size()
{
  //TODO:JK: implement me
}

void
ags_audio_signal_test_copy_buffer_to_buffer()
{
  //TODO:JK: implement me
}

void
ags_audio_signal_test_copy_double_buffer_to_buffer()
{
  //TODO:JK: implement me
}

void
ags_audio_signal_test_duplicate_stream()
{
  //TODO:JK: implement me
}

void
ags_audio_signal_test_get_template()
{
  //TODO:JK: implement me
}

void
ags_audio_signal_test_get_stream_current()
{
  //TODO:JK: implement me
}

void
ags_audio_signal_test_get_by_recall_id()
{
  //TODO:JK: implement me
}

void
ags_audio_signal_test_tile()
{
  //TODO:JK: implement me
}

void
ags_audio_signal_test_scale()
{
  //TODO:JK: implement me
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
  pSuite = CU_add_suite("AgsAudioSignalTest", ags_audio_signal_test_init_suite, ags_audio_signal_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsAudioSignal add stream", ags_audio_signal_test_add_stream) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudioSignal resize stream", ags_audio_signal_test_resize_stream) == NULL)
     /* ||
     (CU_add_test(pSuite, "test of AgsAudioSignal realloc buffer size", ags_audio_signal_test_realloc_buffer_size) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudioSignal copy buffer to buffer", ags_audio_signal_test_copy_buffer_to_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudioSignal copy double buffer to buffer", ags_audio_signal_test_copy_double_buffer_to_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudioSignal duplicate stream", ags_audio_signal_test_duplicate_stream) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudioSignal get template", ags_audio_signal_test_get_template) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudioSignal get stream current", ags_audio_signal_test_get_stream_current) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudioSignal get by recall id", ags_audio_signal_test_get_by_recall_id) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudioSignal tile", ags_audio_signal_test_tile) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudioSignal scale", ags_audio_signal_test_scale) == NULL)
     */ ){
      CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

