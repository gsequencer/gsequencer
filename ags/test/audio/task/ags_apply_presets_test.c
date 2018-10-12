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

int ags_apply_presets_test_init_suite();
int ags_apply_presets_test_clean_suite();

void ags_apply_presets_test_launch_scope_soundcard();
void ags_apply_presets_test_launch_scope_audio();
void ags_apply_presets_test_launch_scope_channel();
void ags_apply_presets_test_launch_scope_audio_signal();

#define AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_PCM_CHANNELS (6)
#define AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_SAMPLERATE (44100)
#define AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_BUFFER_SIZE (512)
#define AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_FORMAT (AGS_SOUNDCARD_SIGNED_24_BIT)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_apply_presets_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_apply_presets_test_clean_suite()
{  
  return(0);
}

void
ags_apply_presets_test_launch_scope_soundcard()
{
  AgsDevout *devout;
  AgsAudio *audio;

  AgsApplyPresets *apply_presets;

  AgsApplicationContext *application_context;

  guint pcm_channels;
  guint samplerate;
  guint buffer_size;
  guint format;
  
  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);

  devout = ags_devout_new(application_context);
  ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				   g_list_append(NULL, devout));
  g_object_ref(devout);

  audio = ags_audio_new(devout);
  ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(application_context),
			       g_list_append(NULL, audio));
  g_object_ref(audio);

  apply_presets = ags_apply_presets_new(devout,
					AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_PCM_CHANNELS,
					AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_SAMPLERATE,
					AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_BUFFER_SIZE,
					AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_FORMAT);

  CU_ASSERT(AGS_IS_APPLY_PRESETS(apply_presets));
  CU_ASSERT(apply_presets->pcm_channels == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_PCM_CHANNELS);
  CU_ASSERT(apply_presets->samplerate == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_SAMPLERATE);
  CU_ASSERT(apply_presets->buffer_size == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_BUFFER_SIZE);
  CU_ASSERT(apply_presets->format == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_FORMAT);
  
  /* test */
  ags_task_launch(apply_presets);

  ags_soundcard_get_presets(AGS_SOUNDCARD(devout),
			    &pcm_channels,
			    &samplerate,
			    &buffer_size,
			    &format);
  
  CU_ASSERT(pcm_channels == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_PCM_CHANNELS);
  CU_ASSERT(samplerate == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_SAMPLERATE);
  CU_ASSERT(buffer_size == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_BUFFER_SIZE);
  CU_ASSERT(format == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_FORMAT);

  g_object_get(audio,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  CU_ASSERT(samplerate == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_SAMPLERATE);
  CU_ASSERT(buffer_size == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_BUFFER_SIZE);
  CU_ASSERT(format == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_FORMAT);
}

void
ags_apply_presets_test_launch_scope_audio()
{
  AgsDevout *devout;
  AgsAudio *audio;
  AgsChannel *channel;
  
  AgsApplyPresets *apply_presets;

  AgsApplicationContext *application_context;

  guint pcm_channels;
  guint samplerate;
  guint buffer_size;
  guint format;
  
  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);

  devout = ags_devout_new(application_context);
  ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				   g_list_append(NULL, devout));
  g_object_ref(devout);

  audio = ags_audio_new(devout);
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
		     1, 0);
  
  apply_presets = ags_apply_presets_new(audio,
					AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_PCM_CHANNELS,
					AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_SAMPLERATE,
					AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_BUFFER_SIZE,
					AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_FORMAT);

  CU_ASSERT(AGS_IS_APPLY_PRESETS(apply_presets));
  CU_ASSERT(apply_presets->pcm_channels == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_PCM_CHANNELS);
  CU_ASSERT(apply_presets->samplerate == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_SAMPLERATE);
  CU_ASSERT(apply_presets->buffer_size == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_BUFFER_SIZE);
  CU_ASSERT(apply_presets->format == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_FORMAT);

  /* test */
  ags_task_launch(apply_presets);
  
  g_object_get(audio,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  CU_ASSERT(samplerate == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_SAMPLERATE);
  CU_ASSERT(buffer_size == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_BUFFER_SIZE);
  CU_ASSERT(format == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_FORMAT);

  g_object_get(audio->output,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  CU_ASSERT(samplerate == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_SAMPLERATE);
  CU_ASSERT(buffer_size == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_BUFFER_SIZE);
  CU_ASSERT(format == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_FORMAT);

  g_object_get(audio->input,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  CU_ASSERT(samplerate == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_SAMPLERATE);
  CU_ASSERT(buffer_size == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_BUFFER_SIZE);
  CU_ASSERT(format == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_FORMAT);
}

void
ags_apply_presets_test_launch_scope_channel()
{
  AgsDevout *devout;
  AgsAudio *audio;
  AgsChannel *channel;
  AgsAudioSignal *audio_signal;  

  AgsApplyPresets *apply_presets;

  AgsApplicationContext *application_context;

  guint pcm_channels;
  guint samplerate;
  guint buffer_size;
  guint format;
  
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
		     1, 0);
  
  audio_signal = ags_audio_signal_new(devout,
				      audio->input->first_recycling,
				      NULL);
  ags_recycling_add_audio_signal(audio->input->first_recycling,
				 audio_signal);

  apply_presets = ags_apply_presets_new(audio->input,
					AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_PCM_CHANNELS,
					AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_SAMPLERATE,
					AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_BUFFER_SIZE,
					AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_FORMAT);

  CU_ASSERT(AGS_IS_APPLY_PRESETS(apply_presets));
  CU_ASSERT(apply_presets->pcm_channels == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_PCM_CHANNELS);
  CU_ASSERT(apply_presets->samplerate == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_SAMPLERATE);
  CU_ASSERT(apply_presets->buffer_size == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_BUFFER_SIZE);
  CU_ASSERT(apply_presets->format == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_FORMAT);

  /* test */
  ags_task_launch(apply_presets);

  g_object_get(audio->input,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  CU_ASSERT(samplerate == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_SAMPLERATE);
  CU_ASSERT(buffer_size == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_BUFFER_SIZE);
  CU_ASSERT(format == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_FORMAT);

  //TODO:JK: implement resample audio data
}

void
ags_apply_presets_test_launch_scope_audio_signal()
{
  AgsDevout *devout;
  AgsAudio *audio;
  AgsChannel *channel;
  AgsAudioSignal *audio_signal;  

  AgsApplyPresets *apply_presets;

  AgsApplicationContext *application_context;

  guint pcm_channels;
  guint samplerate;
  guint buffer_size;
  guint format;
  
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
		     1, 0);
  
  audio_signal = ags_audio_signal_new(devout,
				      audio->input->first_recycling,
				      NULL);
  ags_recycling_add_audio_signal(audio->input->first_recycling,
				 audio_signal);

  apply_presets = ags_apply_presets_new(audio_signal,
					AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_PCM_CHANNELS,
					AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_SAMPLERATE,
					AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_BUFFER_SIZE,
					AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_FORMAT);

  CU_ASSERT(AGS_IS_APPLY_PRESETS(apply_presets));
  CU_ASSERT(apply_presets->pcm_channels == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_PCM_CHANNELS);
  CU_ASSERT(apply_presets->samplerate == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_SAMPLERATE);
  CU_ASSERT(apply_presets->buffer_size == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_BUFFER_SIZE);
  CU_ASSERT(apply_presets->format == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_FORMAT);

  /* test */
  ags_task_launch(apply_presets);

  g_object_get(audio_signal,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  CU_ASSERT(samplerate == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_SAMPLERATE);
  CU_ASSERT(buffer_size == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_BUFFER_SIZE);
  CU_ASSERT(format == AGS_APPLY_PRESETS_TEST_LAUNCH_SCOPE_SOUNDCARD_FORMAT);
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
  pSuite = CU_add_suite("AgsApplyPresetsTest", ags_apply_presets_test_init_suite, ags_apply_presets_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsApplyPresets launch scope soundcard", ags_apply_presets_test_launch_scope_soundcard) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplyPresets launch scope audio", ags_apply_presets_test_launch_scope_audio) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplyPresets launch scope channel", ags_apply_presets_test_launch_scope_channel) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplyPresets launch scope audio signal", ags_apply_presets_test_launch_scope_audio_signal) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
