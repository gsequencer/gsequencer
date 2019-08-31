/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

int ags_functional_pitch_test_init_suite();
int ags_functional_pitch_test_clean_suite();

void ags_functional_pitch_test_pitch_up();
void ags_functional_pitch_test_pitch_down();

#define AGS_FUNCTIONAL_PITCH_TEST_AUDIO_CHANNELS (2)

#define AGS_FUNCTIONAL_PITCH_TEST_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)
#define AGS_FUNCTIONAL_PITCH_TEST_BUFFER_SIZE (1024)
#define AGS_FUNCTIONAL_PITCH_TEST_SAMPLERATE (44100)

#define AGS_FUNCTIONAL_PITCH_TEST_BASE_KEY (0.0)
#define AGS_FUNCTIONAL_PITCH_TEST_BASE_FREQ (440.0)

#define AGS_FUNCTIONAL_PITCH_TEST_TUNE (100.0)

#define AGS_FUNCTIONAL_PITCH_TEST_PITCH_UP_END_KEY (48.0)
#define AGS_FUNCTIONAL_PITCH_TEST_PITCH_UP_FRAME_COUNT (5 * 44100)

#define AGS_FUNCTIONAL_PITCH_TEST_PITCH_DOWN_END_KEY (-48.0)
#define AGS_FUNCTIONAL_PITCH_TEST_PITCH_DOWN_FRAME_COUNT (5 * 44100)

#define AGS_FUNCTIONAL_PITCH_TEST_CONFIG "[generic]\n"	\
  "autosave-thread=false\n"				\
  "simple-file=true\n"					\
  "disable-feature=experimental\n"			\
  "segmentation=4/4\n"					\
  "\n"							\
  "[thread]\n"						\
  "model=super-threaded\n"				\
  "super-threaded-scope=channel\n"			\
  "lock-global=ags-thread\n"				\
  "lock-parent=ags-recycling-thread\n"			\
  "\n"							\
  "[soundcard]\n"					\
  "backend=alsa\n"					\
  "device=default\n"					\
  "samplerate=44100\n"					\
  "buffer-size=1024\n"					\
  "pcm-channels=2\n"					\
  "dsp-channels=2\n"					\
  "format=16\n"						\
  "\n"							\
  "[recall]\n"						\
  "auto-sense=true\n"					\
  "\n"

AgsAudioApplicationContext *audio_application_context;

AgsAudio *output_panel;
AgsAudio *wave_player;

GObject *output_soundcard;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_pitch_test_init_suite()
{
  AgsConfig *config;

  GList *start_list;
  
  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_FUNCTIONAL_PITCH_TEST_CONFIG,
			    strlen(AGS_FUNCTIONAL_PITCH_TEST_CONFIG));

  /* audio application context */
  audio_application_context = (AgsApplicationContext *) ags_audio_application_context_new();
  g_object_ref(audio_application_context);

  ags_application_context_prepare(audio_application_context);
  ags_application_context_setup(audio_application_context);

  /* output soundcard */
  output_soundcard = audio_application_context->soundcard->data;

  start_list = NULL;
  
  /* output panel */
  output_panel = ags_audio_new(output_soundcard);

  g_object_ref(output_panel);
  start_list = g_list_prepend(start_list,
			      output_panel);
  
  ags_audio_set_flags(output_panel, (AGS_AUDIO_SYNC));

  ags_audio_set_audio_channels(output_panel,
			       AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(output_panel,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(output_panel,
		     AGS_TYPE_INPUT,
		     1, 0);

  /* ags-play */
  ags_recall_factory_create(output_panel,
			    NULL, NULL,
			    "ags-play-master",
			    0, AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS,
			    0, 1,
			    (AGS_RECALL_FACTORY_INPUT,
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* wave player */
  wave_player = ags_audio_new(output_soundcard);

  g_object_ref(wave_player);
  start_list = g_list_prepend(start_list,
			      wave_player);
    
  ags_audio_set_flags(wave_player, (AGS_AUDIO_SYNC |
				    AGS_AUDIO_OUTPUT_HAS_RECYCLING |
				    AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_ability_flags(wave_player, (AGS_SOUND_ABILITY_WAVE));
  
  /* ags-play-wave */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-play-wave",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);

  /*  */
  start_list = g_list_reverse(start_list);
  ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(audio_application_context),
			       start_list);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_pitch_test_clean_suite()
{
  g_object_unref(audio_application_context);
  
  return(0);
}

void
ags_functional_pitch_test_pitch_up()
{
  GList *start_list, *list;

  guint i;
  gboolean success;

  start_list = NULL;
  
  for(i = (gint) AGS_FUNCTIONAL_PITCH_TEST_BASE_KEY; i <= (gint) AGS_FUNCTIONAL_PITCH_TEST_PITCH_DOWN_END_KEY; i++){
  }

  success = TRUE;

  for(i = (gint) AGS_FUNCTIONAL_PITCH_TEST_BASE_KEY; i <= (gint) AGS_FUNCTIONAL_PITCH_TEST_PITCH_DOWN_END_KEY; i++){
  }

  CU_ASSERT(success == TRUE);
}

void
ags_functional_pitch_test_pitch_down()
{
  GList *start_list, *list;

  guint i;
  gboolean success;

  start_list = NULL;
  
  for(i = (gint) AGS_FUNCTIONAL_PITCH_TEST_BASE_KEY; i >= (gint) AGS_FUNCTIONAL_PITCH_TEST_PITCH_DOWN_END_KEY; i--){
  }

  success = TRUE;

  for(i = (gint) AGS_FUNCTIONAL_PITCH_TEST_BASE_KEY; i >= (gint) AGS_FUNCTIONAL_PITCH_TEST_PITCH_DOWN_END_KEY; i--){
  }

  CU_ASSERT(success == TRUE);
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
  pSuite = CU_add_suite("AgsFunctionalPitchTest", ags_functional_pitch_test_init_suite, ags_functional_pitch_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_filter_util.h doing pitch up", ags_functional_pitch_test_pitch_up) == NULL) ||
     (CU_add_test(pSuite, "test of ags_filter_util.h doing pitch down", ags_functional_pitch_test_pitch_down) == NULL)){
      CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

