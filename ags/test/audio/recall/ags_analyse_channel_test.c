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

int ags_analyse_channel_test_init_suite();
int ags_analyse_channel_test_clean_suite();

void ags_analyse_channel_test_port();
void ags_analyse_channel_test_buffer_add();
void ags_analyse_channel_test_retrieve_frequency_and_magnitude();

#define AGS_ANALYSE_CHANNEL_TEST_BUFFER_ADD_FREQUENCY (440.0)

#define AGS_ANALYSE_CHANNEL_TEST_RETRIEVE_FREQUENCY_AND_MAGNITUDE_FREQUENCY (440.0)
 
AgsDevout *devout;
AgsAudio *audio;

extern AgsApplicationContext *ags_application_context;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_analyse_channel_test_init_suite()
{ 
  ags_application_context = ags_audio_application_context_new();
  
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

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_analyse_channel_test_clean_suite()
{  
  g_object_run_dispose(devout);
  g_object_unref(devout);

  g_object_run_dispose(audio);
  g_object_unref(audio);

  return(0);
}

void
ags_analyse_channel_test_port()
{
  AgsAnalyseChannel *analyse_channel;
  AgsPort *port;

  analyse_channel = ags_analyse_channel_new(audio->input);

  CU_ASSERT(analyse_channel != NULL);
  CU_ASSERT(AGS_IS_ANALYSE_CHANNEL(analyse_channel));

  /* test ports */
  port = NULL;
  g_object_get(analyse_channel,
	       "buffer-cleared", &port,
	       NULL);

  CU_ASSERT(port != NULL);
  CU_ASSERT(AGS_IS_PORT(port));
  
  port = NULL;
  g_object_get(analyse_channel,
	       "buffer-computed", &port,
	       NULL);

  CU_ASSERT(port != NULL);
  CU_ASSERT(AGS_IS_PORT(port));

  port = NULL;
  g_object_get(analyse_channel,
	       "frequency-buffer", &port,
	       NULL);

  CU_ASSERT(port != NULL);
  CU_ASSERT(AGS_IS_PORT(port));

  port = NULL;
  g_object_get(analyse_channel,
	       "magnitude-buffer", &port,
	       NULL);

  CU_ASSERT(port != NULL);
  CU_ASSERT(AGS_IS_PORT(port));
}

void
ags_analyse_channel_test_buffer_add()
{
  AgsAnalyseChannel *analyse_channel;

  gint16 *s16_buffer;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  analyse_channel = ags_analyse_channel_new(audio->input);

  /* create buffer and fill it */
  s16_buffer = ags_stream_alloc(analyse_channel->cache_buffer_size,
				AGS_SOUNDCARD_SIGNED_16_BIT);
  
  for(i = 0; i < analyse_channel->cache_buffer_size; i++){
    s16_buffer[i] = G_MAXINT16 * sin(i * 2.0 * M_PI * AGS_ANALYSE_CHANNEL_TEST_BUFFER_ADD_FREQUENCY / analyse_channel->cache_samplerate);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s16(s16_buffer,
							  analyse_channel->cache_buffer_size);

  /* test */
  ags_audio_buffer_util_clear_double(analyse_channel->in, 1,
				     analyse_channel->cache_buffer_size);
  ags_analyse_channel_buffer_add(analyse_channel,
				 s16_buffer,
				 analyse_channel->cache_samplerate, analyse_channel->cache_buffer_size, AGS_SOUNDCARD_SIGNED_16_BIT);

  xcross_count = ags_synth_util_get_xcross_count(analyse_channel->in,
						 analyse_channel->cache_format,
						 analyse_channel->cache_buffer_size);

  CU_ASSERT(xcross_count == orig_xcross_count);
}

void
ags_analyse_channel_test_retrieve_frequency_and_magnitude()
{
  AgsAnalyseChannel *analyse_channel;

  gint16 *s16_buffer;

  guint i;
  gboolean is_empty;
  
  analyse_channel = ags_analyse_channel_new(audio->input);
  
  /* create buffer and fill it */
  s16_buffer = ags_stream_alloc(analyse_channel->cache_buffer_size,
				AGS_SOUNDCARD_SIGNED_16_BIT);
  
  for(i = 0; i < analyse_channel->cache_buffer_size; i++){
    s16_buffer[i] = G_MAXINT16 * sin(i * 2.0 * M_PI * AGS_ANALYSE_CHANNEL_TEST_RETRIEVE_FREQUENCY_AND_MAGNITUDE_FREQUENCY / analyse_channel->cache_samplerate);
  }

  ags_analyse_channel_buffer_add(analyse_channel,
				 s16_buffer,
				 analyse_channel->cache_samplerate, analyse_channel->cache_buffer_size, AGS_SOUNDCARD_SIGNED_16_BIT);

  /* test */
  ags_analyse_channel_retrieve_frequency_and_magnitude(analyse_channel);

  is_empty = TRUE;
  
  for(i = 0; i < ceil(analyse_channel->cache_buffer_size / 2.0); i++){
    if(analyse_channel->magnitude_pre_buffer[i] != 0.0){
      is_empty = FALSE;

      break;
    }
  }

  CU_ASSERT(is_empty == FALSE);
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
  pSuite = CU_add_suite("AgsAnalyseChannelTest", ags_analyse_channel_test_init_suite, ags_analyse_channel_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsAnalyseChannel port", ags_analyse_channel_test_port) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAnalyseChannel buffer add", ags_analyse_channel_test_buffer_add) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAnalyseChannel retrieve frequency and magnitude", ags_analyse_channel_test_retrieve_frequency_and_magnitude) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
