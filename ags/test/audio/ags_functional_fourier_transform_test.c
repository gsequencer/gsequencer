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

int ags_functional_fourier_transform_test_init_suite();
int ags_functional_fourier_transform_test_clean_suite();

void ags_functional_fourier_transform_test_s16();

#define AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_AUDIO_CHANNELS (2)

#define AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_SAMPLERATE (44100)
#define AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_BUFFER_SIZE (1024)
#define AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

#define AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_AUDIO_CHANNELS (2)

#define AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_SAMPLERATE (44100)
#define AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_BUFFER_SIZE (1024)
#define AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

#define AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_VOLUME (1.0)
#define AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_BASE_FREQ (440.0)

#define AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_FRAME_COUNT (5 * 44100)

#define AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_DELAY (8000000)

#define AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_CONFIG "[generic]\n"	\
  "autosave-thread=false\n"						\
  "simple-file=true\n"							\
  "disable-feature=experimental\n"					\
  "segmentation=4/4\n"							\
  "\n"									\
  "[thread]\n"								\
  "model=super-threaded\n"						\
  "super-threaded-scope=channel\n"					\
  "lock-global=ags-thread\n"						\
  "lock-parent=ags-recycling-thread\n"					\
  "\n"									\
  "[soundcard]\n"							\
  "backend=alsa\n"							\
  "device=default\n"							\
  "samplerate=44100\n"							\
  "buffer-size=1024\n"							\
  "pcm-channels=2\n"							\
  "dsp-channels=2\n"							\
  "format=16\n"								\
  "\n"									\
  "[recall]\n"								\
  "auto-sense=true\n"							\
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
ags_functional_fourier_transform_test_init_suite()
{
  AgsChannel *channel, *link;
  
  AgsConfig *config;

  GList *start_list;

  guint i;
  
  GError *error;
  
  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_CONFIG,
			    strlen(AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_CONFIG));

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
			       AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_AUDIO_CHANNELS, 0);
  
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
			    0, AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_AUDIO_CHANNELS,
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
  
  ags_audio_set_audio_channels(wave_player,
			       AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(wave_player,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(wave_player,
		     AGS_TYPE_INPUT,
		     1, 0);

  channel = wave_player->output;

  for(i = 0; i < AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_AUDIO_CHANNELS; i++){
    ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_WAVE));

    channel = channel->next;
  }
  
  /* ags-play-wave */
  ags_recall_factory_create(wave_player,
			    NULL, NULL,
			    "ags-play-wave",
			    0, AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_AUDIO_CHANNELS,
			    0, 1,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);

  /*  */
  start_list = g_list_reverse(start_list);
  ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(audio_application_context),
			       start_list);

  /* link */
  channel = output_panel->input;
  link = wave_player->output;
  
  for(i = 0; i < AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_AUDIO_CHANNELS; i++){
    error = NULL;
    ags_channel_set_link(channel,
			 link,
			 &error);

    channel = channel->next;
    link = link->next;
  }
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_fourier_transform_test_clean_suite()
{
  g_object_unref(audio_application_context);
  
  return(0);
}

void
ags_functional_fourier_transform_test_s16()
{
  AgsStartAudio *start_audio;
  AgsStartSoundcard *start_soundcard;
  AgsCancelAudio *cancel_audio;

  AgsTaskLauncher *task_launcher;
  
  GList *start_wave, *wave;
  GList *start_buffer, *buffer;
  GList *task;

  AgsComplex **ptr_ptr_val;
  AgsComplex *ptr_val;
  AgsComplex val;
  gint16 **ptr_ptr_data;
  gint16 *ptr_data;

  gint i, j, k;
  gboolean success;
    
  start_wave = NULL;
  
  for(i = 0; i < AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_AUDIO_CHANNELS; i++){
    AgsWave *current_wave;
    
    current_wave = ags_wave_new(wave_player,
				i);

    g_object_set(current_wave,
		 "samplerate", AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_SAMPLERATE,
		 "buffer-size", AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_BUFFER_SIZE,
		 "format", AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_FORMAT,
		 NULL);

    start_wave = ags_wave_add(start_wave,
			      current_wave);
    
    for(j = 0; j < floor(AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_FRAME_COUNT / AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_BUFFER_SIZE); j++){
      AgsBuffer *buffer;

      gdouble phase;
      
      buffer = ags_buffer_new();
      g_object_set(buffer,
		   "samplerate", AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_SAMPLERATE,
		   "buffer-size", AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_BUFFER_SIZE,
		   "format", AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_FORMAT,
		   "x", j * AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_BUFFER_SIZE,
		   NULL);
      ags_wave_add_buffer(current_wave,
			  buffer,
			  FALSE);

      phase = buffer->x % (guint) floor(AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_SAMPLERATE / AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_BASE_FREQ);
      ags_synth_util_sin(buffer->data,
			 AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_BASE_FREQ, phase, AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_VOLUME,
			 AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_SAMPLERATE, ags_audio_buffer_util_format_from_soundcard(AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_FORMAT),
			 0, AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_BUFFER_SIZE);
    }
  }
  
  wave_player->wave = start_wave;
  
  g_message("playing frequency = %f", AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_BASE_FREQ);

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(audio_application_context));

  /* start audio and soundcard task */
  task = NULL;    
  start_audio = ags_start_audio_new(wave_player,
				    AGS_SOUND_SCOPE_WAVE);
  task = g_list_prepend(task,
			start_audio);
    
  start_soundcard = ags_start_soundcard_new(audio_application_context);
  task = g_list_prepend(task,
			start_soundcard);
    
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  /* delay */
  usleep(AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_DELAY);
    
  /* create cancel task */
  cancel_audio = ags_cancel_audio_new(wave_player,
				      AGS_SOUND_SCOPE_WAVE);
    
  /* append AgsCancelAudio */
  ags_task_launcher_add_task(task_launcher,
			     (AgsTask *) cancel_audio);

  /*  */
  success = TRUE;

  wave = start_wave;

  ptr_val = &val;
  ptr_ptr_val = &ptr_val;

  ptr_ptr_data = &ptr_data;
  
  for(i = 0; i < AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_AUDIO_CHANNELS; i++){
    AgsWave *current_wave;
    
    current_wave = wave->data;

    buffer =
      start_buffer = current_wave->buffer;
    
    for(j = 0; j < floor(AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_FRAME_COUNT / AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_BUFFER_SIZE); j++){
      AgsBuffer *current_buffer;

      gint16 *data;
      
      current_buffer = buffer->data;

      data = current_buffer->data;
      
      for(k = 0; k < AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_BUFFER_SIZE; k++){
	ptr_data = data + k;

	AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S16_FRAME(ptr_data, 1,
							  k, AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_BUFFER_SIZE,
							  ptr_ptr_val);


	AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S16_FRAME(ptr_val, 1,
							  k, AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_BUFFER_SIZE,
							  ptr_ptr_data);
      }
    }
  }

  g_message("playing frequency with transformation = %f", AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_BASE_FREQ);

  /* start audio and soundcard task */
  task = NULL;    
  start_audio = ags_start_audio_new(wave_player,
				    AGS_SOUND_SCOPE_WAVE);
  task = g_list_prepend(task,
			start_audio);
    
  start_soundcard = ags_start_soundcard_new(audio_application_context);
  task = g_list_prepend(task,
			start_soundcard);
    
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  /* delay */
  usleep(AGS_FUNCTIONAL_FOURIER_TRANSFORM_TEST_S16_DELAY);
    
  /* create cancel task */
  cancel_audio = ags_cancel_audio_new(wave_player,
				      AGS_SOUND_SCOPE_WAVE);
    
  /* append AgsCancelAudio */
  ags_task_launcher_add_task(task_launcher,
			     (AgsTask *) cancel_audio);

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
  pSuite = CU_add_suite("AgsFunctionalFourierTransformTest", ags_functional_fourier_transform_test_init_suite, ags_functional_fourier_transform_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_fourier_transform_util.h doing signed 16 bit", ags_functional_fourier_transform_test_s16) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

