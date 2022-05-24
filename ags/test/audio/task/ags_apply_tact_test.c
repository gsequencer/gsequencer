/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

int ags_apply_tact_test_init_suite();
int ags_apply_tact_test_clean_suite();

void ags_apply_tact_test_launch_scope_recall();
void ags_apply_tact_test_launch_scope_channel();
void ags_apply_tact_test_launch_scope_audio();
void ags_apply_tact_test_launch_scope_soundcard();
void ags_apply_tact_test_launch_scope_sequencer();
void ags_apply_tact_test_launch_scope_application_context();

#define AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_RECALL_TACT (1.0)

#define AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_CHANNEL_TACT (1.0)

#define AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_AUDIO_TACT (1.0)

#define AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_SOUNDCARD_TACT (1.0)

#define AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_SEQUENCER_TACT (1.0)

#define AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_APPLICATION_CONTEXT_TACT (1.0)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_apply_tact_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_apply_tact_test_clean_suite()
{  
  return(0);
}

void
ags_apply_tact_test_launch_scope_recall()
{
  AgsAlsaDevout *devout;
  AgsAudio *audio;

  AgsApplyTact *apply_tact;

  devout = ags_alsa_devout_new(NULL);
  g_object_ref(devout);
  
  audio = ags_audio_new(devout);
  g_object_ref(audio);
  
  apply_tact = ags_apply_tact_new(NULL,
				  AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_RECALL_TACT);
  
  CU_ASSERT(AGS_IS_APPLY_TACT(apply_tact));
  CU_ASSERT(apply_tact->scope == NULL);
  CU_ASSERT(apply_tact->tact == AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_RECALL_TACT);
  
  /* test */
  ags_task_launch(apply_tact);
}

void
ags_apply_tact_test_launch_scope_channel()
{
  AgsChannel *channel;

  AgsApplyTact *apply_tact;
  
  channel = ags_channel_new(NULL);
  g_object_ref(channel);

  apply_tact = ags_apply_tact_new(channel,
				AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_CHANNEL_TACT);
  
  CU_ASSERT(AGS_IS_APPLY_TACT(apply_tact));
  CU_ASSERT(apply_tact->scope == channel);
  CU_ASSERT(apply_tact->tact == AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_CHANNEL_TACT);
  
  /* test */
  ags_task_launch(apply_tact);
}

void
ags_apply_tact_test_launch_scope_audio()
{
  AgsAlsaDevout *devout;
  AgsAudio *audio;

  AgsApplyTact *apply_tact;
  
  devout = ags_alsa_devout_new(NULL);
  g_object_ref(devout);

  audio = ags_audio_new(NULL);
  g_object_ref(audio);
  
  apply_tact = ags_apply_tact_new(audio,
				  AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_AUDIO_TACT);
  
  CU_ASSERT(AGS_IS_APPLY_TACT(apply_tact));
  CU_ASSERT(apply_tact->scope == audio);
  CU_ASSERT(apply_tact->tact == AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_AUDIO_TACT);
  
  /* test */
  ags_task_launch(apply_tact);
}

void
ags_apply_tact_test_launch_scope_soundcard()
{
  AgsAlsaDevout *devout;

  AgsApplyTact *apply_tact;
  
  devout = ags_alsa_devout_new();
  g_object_ref(devout);

  apply_tact = ags_apply_tact_new(devout,
				  AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_SOUNDCARD_TACT);
  
  CU_ASSERT(AGS_IS_APPLY_TACT(apply_tact));
  CU_ASSERT(apply_tact->scope == devout);
  CU_ASSERT(apply_tact->tact == AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_SOUNDCARD_TACT);

  /* test */
  ags_task_launch(apply_tact);

  CU_ASSERT(ags_soundcard_get_delay_factor(AGS_SOUNDCARD(devout)) == AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_SOUNDCARD_TACT);
}

void
ags_apply_tact_test_launch_scope_sequencer()
{
  AgsAlsaMidiin *midiin;

  AgsApplyTact *apply_tact;
  
  midiin = ags_alsa_midiin_new();
  g_object_ref(midiin);

  apply_tact = ags_apply_tact_new(midiin,
				AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_SEQUENCER_TACT);
  
  CU_ASSERT(AGS_IS_APPLY_TACT(apply_tact));
  CU_ASSERT(apply_tact->scope == midiin);
  CU_ASSERT(apply_tact->tact == AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_SEQUENCER_TACT);

  /* test */
  ags_task_launch(apply_tact);

  CU_ASSERT(ags_sequencer_get_delay_factor(AGS_SEQUENCER(midiin)) == AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_SEQUENCER_TACT);
}

void
ags_apply_tact_test_launch_scope_application_context()
{
  AgsAlsaDevout *devout;
  AgsAlsaMidiin *midiin;
  AgsAudio *audio;

  AgsApplyTact *apply_tact;

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

  apply_tact = ags_apply_tact_new(application_context,
				AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_APPLICATION_CONTEXT_TACT);

  /* test */
  ags_task_launch(apply_tact);

  CU_ASSERT(ags_soundcard_get_delay_factor(AGS_SOUNDCARD(devout)) == AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_APPLICATION_CONTEXT_TACT);
  CU_ASSERT(ags_sequencer_get_delay_factor(AGS_SEQUENCER(midiin)) == AGS_APPLY_TACT_TEST_LAUNCH_SCOPE_APPLICATION_CONTEXT_TACT);
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
  pSuite = CU_add_suite("AgsApplyTactTest", ags_apply_tact_test_init_suite, ags_apply_tact_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsApplyTact launch scope recall", ags_apply_tact_test_launch_scope_recall) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplyTact launch scope channel", ags_apply_tact_test_launch_scope_channel) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplyTact launch scope audio", ags_apply_tact_test_launch_scope_audio) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplyTact launch scope soundcard", ags_apply_tact_test_launch_scope_soundcard) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplyTact launch scope sequencer", ags_apply_tact_test_launch_scope_sequencer) == NULL) ||
     (CU_add_test(pSuite, "test of AgsApplyTact launch scope application context", ags_apply_tact_test_launch_scope_application_context) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
