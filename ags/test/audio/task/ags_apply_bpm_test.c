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

int ags_apply_bpm_test_init_suite();
int ags_apply_bpm_test_clean_suite();

void ags_apply_bpm_test_launch_scope_recall();
void ags_apply_bpm_test_launch_scope_channel();
void ags_apply_bpm_test_launch_scope_audio();
void ags_apply_bpm_test_launch_scope_soundcard();
void ags_apply_bpm_test_launch_scope_sequencer();
void ags_apply_bpm_test_launch_scope_application_context();

#define AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_RECALL_BPM (145.0)

#define AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_CHANNEL_BPM (145.0)

#define AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_AUDIO_BPM (145.0)

#define AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_SOUNDCARD_BPM (145.0)

#define AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_SEQUENCER_BPM (145.0)

#define AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_APPLICATION_CONTEXT_BPM (145.0)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_apply_bpm_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_apply_bpm_test_clean_suite()
{  
  return(0);
}

void
ags_apply_bpm_test_launch_scope_recall()
{
  AgsAlsaDevout *devout;
  AgsAudio *audio;

  AgsApplyBpm *apply_bpm;

  devout = ags_alsa_devout_new(NULL);
  g_object_ref(devout);
  
  audio = ags_audio_new(devout);
  g_object_ref(audio);
  
  apply_bpm = ags_apply_bpm_new(NULL,
				AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_RECALL_BPM);
  
  CU_ASSERT(AGS_IS_APPLY_BPM(apply_bpm));
  CU_ASSERT(apply_bpm->scope == NULL);
  CU_ASSERT(apply_bpm->bpm == AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_RECALL_BPM);
  
  /* test */
  ags_task_launch(apply_bpm);
}

void
ags_apply_bpm_test_launch_scope_channel()
{
  AgsChannel *channel;

  AgsApplyBpm *apply_bpm;
  
  channel = ags_channel_new(NULL);
  g_object_ref(channel);

  apply_bpm = ags_apply_bpm_new(channel,
				AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_CHANNEL_BPM);
  
  CU_ASSERT(AGS_IS_APPLY_BPM(apply_bpm));
  CU_ASSERT(apply_bpm->scope == channel);
  CU_ASSERT(apply_bpm->bpm == AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_CHANNEL_BPM);
  
  /* test */
  ags_task_launch(apply_bpm);
}

void
ags_apply_bpm_test_launch_scope_audio()
{
  AgsAlsaDevout *devout;
  AgsAudio *audio;

  AgsApplyBpm *apply_bpm;
  
  devout = ags_alsa_devout_new(NULL);
  g_object_ref(devout);

  audio = ags_audio_new(NULL);
  g_object_ref(audio);
  
  apply_bpm = ags_apply_bpm_new(audio,
				AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_AUDIO_BPM);
  
  CU_ASSERT(AGS_IS_APPLY_BPM(apply_bpm));
  CU_ASSERT(apply_bpm->scope == audio);
  CU_ASSERT(apply_bpm->bpm == AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_AUDIO_BPM);
  
  /* test */
  ags_task_launch(apply_bpm);

  CU_ASSERT(audio->bpm == AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_AUDIO_BPM);
}

void
ags_apply_bpm_test_launch_scope_soundcard()
{
  AgsAlsaDevout *devout;

  AgsApplyBpm *apply_bpm;
  
  devout = ags_alsa_devout_new(NULL);
  g_object_ref(devout);

  apply_bpm = ags_apply_bpm_new(devout,
				AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_SOUNDCARD_BPM);
  
  CU_ASSERT(AGS_IS_APPLY_BPM(apply_bpm));
  CU_ASSERT(apply_bpm->scope == devout);
  CU_ASSERT(apply_bpm->bpm == AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_SOUNDCARD_BPM);

  /* test */
  ags_task_launch(apply_bpm);

  CU_ASSERT(ags_soundcard_get_bpm(AGS_SOUNDCARD(devout)) == AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_SOUNDCARD_BPM);
}

void
ags_apply_bpm_test_launch_scope_sequencer()
{
  AgsAlsaMidiin *midiin;

  AgsApplyBpm *apply_bpm;
  
  midiin = ags_alsa_midiin_new(NULL);
  g_object_ref(midiin);

  apply_bpm = ags_apply_bpm_new(midiin,
				AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_SEQUENCER_BPM);
  
  CU_ASSERT(AGS_IS_APPLY_BPM(apply_bpm));
  CU_ASSERT(apply_bpm->scope == midiin);
  CU_ASSERT(apply_bpm->bpm == AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_SEQUENCER_BPM);

  /* test */
  ags_task_launch(apply_bpm);

  CU_ASSERT(ags_sequencer_get_bpm(AGS_SEQUENCER(midiin)) == AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_SEQUENCER_BPM);
}

void
ags_apply_bpm_test_launch_scope_application_context()
{
  AgsAlsaDevout *devout;
  AgsAlsaMidiin *midiin;
  AgsAudio *audio;

  AgsApplyBpm *apply_bpm;

  AgsApplicationContext *application_context;
  
  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);

  devout = ags_alsa_devout_new(application_context);
  ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				   g_list_append(NULL, devout));
  g_object_ref(devout);

  midiin = ags_alsa_midiin_new(application_context);
  ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				   g_list_append(NULL, midiin));
  g_object_ref(midiin);
  
  audio = ags_audio_new(devout);
  ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(application_context),
			       g_list_append(NULL, audio));
  g_object_ref(audio);

  apply_bpm = ags_apply_bpm_new(application_context,
				AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_APPLICATION_CONTEXT_BPM);

  /* test */
  ags_task_launch(apply_bpm);

  CU_ASSERT(ags_soundcard_get_bpm(AGS_SOUNDCARD(devout)) == AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_APPLICATION_CONTEXT_BPM);
  CU_ASSERT(ags_sequencer_get_bpm(AGS_SEQUENCER(midiin)) == AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_APPLICATION_CONTEXT_BPM);
  CU_ASSERT(audio->bpm == AGS_APPLY_BPM_TEST_LAUNCH_SCOPE_APPLICATION_CONTEXT_BPM);
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
  pSuite = CU_add_suite("AgsApplyBpmTest", ags_apply_bpm_test_init_suite, ags_apply_bpm_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsApplyBpm launch scope recall", ags_apply_bpm_test_launch_scope_recall) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplyBpm launch scope channel", ags_apply_bpm_test_launch_scope_channel) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplyBpm launch scope audio", ags_apply_bpm_test_launch_scope_audio) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplyBpm launch scope soundcard", ags_apply_bpm_test_launch_scope_soundcard) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplyBpm launch scope sequencer", ags_apply_bpm_test_launch_scope_sequencer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplyBpm launch scope application context", ags_apply_bpm_test_launch_scope_application_context) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
