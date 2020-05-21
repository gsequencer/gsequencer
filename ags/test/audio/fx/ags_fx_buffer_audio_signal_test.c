/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

int ags_fx_buffer_audio_signal_test_init_suite();
int ags_fx_buffer_audio_signal_test_clean_suite();

void ags_fx_buffer_audio_signal_test_new();
void ags_fx_buffer_audio_signal_test_run_inter();

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_buffer_audio_signal_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fx_buffer_audio_signal_test_clean_suite()
{  
  return(0);
}

void
ags_fx_buffer_audio_signal_test_new()
{
  AgsAudioSignal *audio_signal;
  AgsFxBufferAudioSignal *fx_buffer_audio_signal;

  audio_signal = g_object_new(AGS_TYPE_AUDIO_SIGNAL,
			      NULL);
  
  fx_buffer_audio_signal = ags_fx_buffer_audio_signal_new(audio_signal);

  CU_ASSERT(fx_buffer_audio_signal != NULL);
  CU_ASSERT(AGS_RECALL_AUDIO_SIGNAL(fx_buffer_audio_signal)->source == audio_signal);
}

void
ags_fx_buffer_audio_signal_test_run_inter()
{
  AgsAudio *audio;
  AgsChannel *destination_channel, *source_channel;
  AgsRecycling *destination_recycling, *source_recycling;
  AgsAudioSignal *destination, *source;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context;
  AgsRecallContainer *recall_container;
  AgsFxBufferAudio *fx_buffer_audio;
  AgsFxBufferAudioProcessor *fx_buffer_audio_processor;
  AgsFxBufferChannel *fx_buffer_channel;
  AgsFxBufferChannelProcessor *fx_buffer_channel_processor;
  AgsFxBufferRecycling *fx_buffer_recycling;
  AgsFxBufferAudioSignal *fx_buffer_audio_signal;

  recall_id = ags_recall_id_new();
  ags_recall_id_set_sound_scope(recall_id, AGS_SOUND_SCOPE_PLAYBACK);

  recycling_context = ags_recycling_context_new(1);
  g_object_set(recall_id,
	       "recycling-context", recycling_context,
	       NULL);
  
  /* audio */
  audio = g_object_new(AGS_TYPE_AUDIO,
		       NULL);

  recall_container = ags_recall_container_new();
  ags_audio_add_recall_container(audio,
				 recall_container);
  
  fx_buffer_audio = ags_fx_buffer_audio_new(audio);
  ags_recall_set_sound_scope(fx_buffer_audio, AGS_SOUND_SCOPE_PLAYBACK);
  ags_recall_container_add(recall_container,
			   fx_buffer_audio);
  
  CU_ASSERT(fx_buffer_audio != NULL);

  /* audio processor */  
  fx_buffer_audio_processor = ags_fx_buffer_audio_processor_new(audio);
  g_object_set(fx_buffer_audio_processor,
	       "recall-id", recall_id,
	       NULL);
  ags_recall_set_sound_scope(fx_buffer_audio_processor, AGS_SOUND_SCOPE_PLAYBACK);

  g_object_set(fx_buffer_audio_processor,
	       "recall-audio", fx_buffer_audio,
	       NULL);
  
  ags_recall_container_add(recall_container,
			   fx_buffer_audio_processor);

  CU_ASSERT(fx_buffer_audio_processor != NULL);

  /* channel */
  source_channel = g_object_new(AGS_TYPE_CHANNEL,
				NULL);
  ags_channel_add_recall_container(source_channel,
				   recall_container);

  destination_channel = g_object_new(AGS_TYPE_CHANNEL,
				     NULL);
  
  fx_buffer_channel = ags_fx_buffer_channel_new(source_channel);
  g_object_set(fx_buffer_channel,
	       "destination", destination_channel,
	       NULL);
  ags_recall_set_sound_scope(fx_buffer_channel, AGS_SOUND_SCOPE_PLAYBACK);

  g_object_set(fx_buffer_channel,
	       "recall-audio", fx_buffer_audio,
	       NULL);

  ags_recall_container_add(recall_container,
			   fx_buffer_channel);

  CU_ASSERT(fx_buffer_channel != NULL);

  /* channel processor */  
  fx_buffer_channel_processor = ags_fx_buffer_channel_processor_new(source_channel);
  g_object_set(fx_buffer_channel_processor,
	       "recall-id", recall_id,
	       NULL);
  g_object_set(fx_buffer_channel_processor,
	       "destination", destination_channel,
	       NULL);
  ags_recall_set_sound_scope(fx_buffer_channel_processor, AGS_SOUND_SCOPE_PLAYBACK);

  g_object_set(fx_buffer_channel_processor,
	       "recall-audio", fx_buffer_audio,
	       "recall-channel", fx_buffer_channel,
	       NULL);

  ags_recall_container_add(recall_container,
			   fx_buffer_channel_processor);

  CU_ASSERT(fx_buffer_channel_processor != NULL);

  /* recycling */
  source_recycling = g_object_new(AGS_TYPE_RECYCLING,
				  NULL);
  ags_recycling_context_insert(recycling_context,
			       source_recycling,
			       0);
  
  destination_recycling = g_object_new(AGS_TYPE_RECYCLING,
				       NULL);
  
  fx_buffer_recycling = ags_fx_buffer_recycling_new(source_recycling);
  g_object_set(fx_buffer_recycling,
	       "recall-id", recall_id,
	       NULL);

  g_object_set(fx_buffer_recycling,
	       "destination", destination_recycling,
	       NULL);
  
  ags_recall_set_sound_scope(fx_buffer_recycling, AGS_SOUND_SCOPE_PLAYBACK);

  ags_recall_add_child(fx_buffer_channel_processor,
		       fx_buffer_recycling);
  
  CU_ASSERT(fx_buffer_recycling != NULL);

  /* audio signal */
  source = g_object_new(AGS_TYPE_AUDIO_SIGNAL,
			NULL);

  //NOTE:JK: survive all attempts
  ags_audio_signal_stream_resize(source,
				 7);
  source->stream_current = source->stream;

  destination = g_object_new(AGS_TYPE_AUDIO_SIGNAL,
			     NULL);
  ags_audio_signal_set_flags(destination,
			     AGS_AUDIO_SIGNAL_MASTER);
  
  ags_audio_signal_stream_resize(destination,
				 1);
  destination->stream_current = destination->stream;
  
  fx_buffer_audio_signal = ags_fx_buffer_audio_signal_new(source);
  g_object_set(fx_buffer_audio_signal,
	       "recall-id", recall_id,
	       NULL);
  g_object_set(fx_buffer_audio_signal,
	       "destination", destination,
	       NULL);
  ags_recall_set_sound_scope(fx_buffer_audio_signal, AGS_SOUND_SCOPE_PLAYBACK);
  ags_recall_set_flags(fx_buffer_audio_signal, AGS_RECALL_INITIAL_RUN);

  ags_recall_add_child(fx_buffer_recycling,
		       fx_buffer_audio_signal);

  CU_ASSERT(fx_buffer_audio_signal != NULL);

  /* run inter - attempt #0 */
  ags_recall_run_inter(fx_buffer_audio_signal);

  /* run inter - attempt #1 */
  ags_recall_unset_flags(fx_buffer_audio_signal, AGS_RECALL_INITIAL_RUN);

  ags_recall_run_inter(fx_buffer_audio_signal);

  /* run inter - attempt #2 */
  AGS_RECALL_AUDIO_SIGNAL(fx_buffer_audio_signal)->destination = NULL;
  
  ags_recall_run_inter(fx_buffer_audio_signal);

  /* run inter - attempt #3 */
  ags_recall_set_flags(fx_buffer_audio_signal, AGS_RECALL_INITIAL_RUN);

  destination = g_object_new(AGS_TYPE_AUDIO_SIGNAL,
			     NULL);
  ags_audio_signal_set_flags(destination,
			     AGS_AUDIO_SIGNAL_MASTER);
  g_object_set(destination,
	       "samplerate", 8000,
	       NULL);
  
  ags_audio_signal_stream_resize(destination,
				 1);
  destination->stream_current = destination->stream;
  source->stream_current = source->stream;

  AGS_RECALL_AUDIO_SIGNAL(fx_buffer_audio_signal)->destination = destination;
  
  ags_recall_run_inter(fx_buffer_audio_signal);

  /* run inter - attempt #4 */
  ags_recall_unset_flags(fx_buffer_audio_signal, AGS_RECALL_INITIAL_RUN);

  ags_recall_run_inter(fx_buffer_audio_signal);

  /* run inter - attempt #5 */
  source->stream_current = NULL;

  ags_recall_run_inter(fx_buffer_audio_signal);
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
  pSuite = CU_add_suite("AgsFxBufferAudioSignalTest", ags_fx_buffer_audio_signal_test_init_suite, ags_fx_buffer_audio_signal_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFxBufferAudioSignal new", ags_fx_buffer_audio_signal_test_new) == NULL) ||
     (CU_add_test(pSuite, "test of AgsFxBufferAudioSignal run inter", ags_fx_buffer_audio_signal_test_run_inter) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
