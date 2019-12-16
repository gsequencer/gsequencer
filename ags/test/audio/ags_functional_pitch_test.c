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

struct AgsFunctionalPitchTestWave
{
  gdouble freq;
  gdouble key;
  
  GList *wave;
};

int ags_functional_pitch_test_init_suite();
int ags_functional_pitch_test_clean_suite();

void ags_functional_pitch_test_pitch_up();
void ags_functional_pitch_test_pitch_down();

struct AgsFunctionalPitchTestWave* ags_functional_pitch_test_alloc(GList *template_wave);

#define AGS_FUNCTIONAL_PITCH_TEST_AUDIO_CHANNELS (2)

#define AGS_FUNCTIONAL_PITCH_TEST_SAMPLERATE (44100)
#define AGS_FUNCTIONAL_PITCH_TEST_BUFFER_SIZE (1024)
#define AGS_FUNCTIONAL_PITCH_TEST_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

#define AGS_FUNCTIONAL_PITCH_TEST_VOLUME (1.0)

#define AGS_FUNCTIONAL_PITCH_TEST_BASE_KEY (0.0)
#define AGS_FUNCTIONAL_PITCH_TEST_BASE_FREQ (440.0)

#define AGS_FUNCTIONAL_PITCH_TEST_TUNE (100.0)

#define AGS_FUNCTIONAL_PITCH_TEST_DELAY (8000000)

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
  "super-threaded-scope=audio\n"			\
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
  AgsChannel *channel, *link;
  
  AgsConfig *config;

  GList *start_list;

  guint i;
  
  GError *error;
  
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
			       AGS_FUNCTIONAL_PITCH_TEST_AUDIO_CHANNELS, 0);
  
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
			    0, AGS_FUNCTIONAL_PITCH_TEST_AUDIO_CHANNELS,
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
			       AGS_FUNCTIONAL_PITCH_TEST_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(wave_player,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(wave_player,
		     AGS_TYPE_INPUT,
		     1, 0);

  channel = wave_player->output;

  for(i = 0; i < AGS_FUNCTIONAL_PITCH_TEST_AUDIO_CHANNELS; i++){
    ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_WAVE));

    channel = channel->next;
  }
  
  /* ags-play-wave */
  ags_recall_factory_create(wave_player,
			    NULL, NULL,
			    "ags-play-wave",
			    0, AGS_FUNCTIONAL_PITCH_TEST_AUDIO_CHANNELS,
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
  
  for(i = 0; i < AGS_FUNCTIONAL_PITCH_TEST_AUDIO_CHANNELS; i++){
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
ags_functional_pitch_test_clean_suite()
{
  g_object_unref(audio_application_context);
  
  return(0);
}

struct AgsFunctionalPitchTestWave*
ags_functional_pitch_test_alloc(GList *template_wave)
{
  struct AgsFunctionalPitchTestWave *test_wave;

  guint i_stop, j_stop;
  guint i, j;
  
  test_wave = (struct AgsFunctionalPitchTestWave *) malloc(sizeof(struct AgsFunctionalPitchTestWave));

  test_wave->key = AGS_FUNCTIONAL_PITCH_TEST_BASE_KEY;
  test_wave->freq = AGS_FUNCTIONAL_PITCH_TEST_BASE_FREQ;

  test_wave->wave = NULL;

  /* wave */
  i_stop = g_list_length(template_wave);

  for(i = 0; i < i_stop; i++){
    AgsWave *wave, *current_wave;

    GList *template_buffer;
    
    current_wave = AGS_WAVE(template_wave->data);

    /* instantiate wave */
    wave = ags_wave_new(current_wave->audio,
			current_wave->line);

    ags_timestamp_set_ags_offset(wave,
				 ags_timestamp_get_ags_offset(current_wave));
    
    g_object_set(wave,
		 "samplerate", AGS_FUNCTIONAL_PITCH_TEST_SAMPLERATE,
		 "buffer-size", AGS_FUNCTIONAL_PITCH_TEST_BUFFER_SIZE,
		 "format", AGS_FUNCTIONAL_PITCH_TEST_FORMAT,
		 NULL);

    test_wave->wave = ags_wave_add(test_wave->wave,
				   wave);
    
    /* buffer */
    template_buffer = current_wave->buffer;
    
    j_stop = g_list_length(current_wave->buffer);
    
    for(j = 0; j < j_stop; j++){
      AgsBuffer *buffer, *current_buffer;

      current_buffer = AGS_BUFFER(template_buffer->data);

      /* duplicate buffer */
      buffer = ags_buffer_duplicate(current_buffer);
      ags_wave_add_buffer(wave,
			  buffer,
			  FALSE);

      /* iterate buffer */
      template_buffer = template_buffer->next;
    }

    /* iterate wave */
    template_wave = template_wave->next;
  }
  
  return(test_wave);
}

void
ags_functional_pitch_test_pitch_up()
{
  AgsTaskLauncher *task_launcher;
  
  GList *start_wave, *wave;
  GList *start_list, *list;

  gint i, j, k;
  gboolean success;
  
  start_wave = NULL;
  
  for(i = 0; i < AGS_FUNCTIONAL_PITCH_TEST_AUDIO_CHANNELS; i++){
    AgsWave *current_wave;
    
    current_wave = ags_wave_new(wave_player,
				i);

    g_object_set(current_wave,
		 "samplerate", AGS_FUNCTIONAL_PITCH_TEST_SAMPLERATE,
		 "buffer-size", AGS_FUNCTIONAL_PITCH_TEST_BUFFER_SIZE,
		 "format", AGS_FUNCTIONAL_PITCH_TEST_FORMAT,
		 NULL);

    start_wave = ags_wave_add(start_wave,
			      current_wave);
    
    for(j = 0; j < floor(AGS_FUNCTIONAL_PITCH_TEST_PITCH_UP_FRAME_COUNT / AGS_FUNCTIONAL_PITCH_TEST_BUFFER_SIZE); j++){
      AgsBuffer *buffer;

      gdouble phase;
      
      buffer = ags_buffer_new();
      g_object_set(buffer,
		   "samplerate", AGS_FUNCTIONAL_PITCH_TEST_SAMPLERATE,
		   "buffer-size", AGS_FUNCTIONAL_PITCH_TEST_BUFFER_SIZE,
		   "format", AGS_FUNCTIONAL_PITCH_TEST_FORMAT,
		   "x", j * AGS_FUNCTIONAL_PITCH_TEST_BUFFER_SIZE,
		   NULL);
      ags_wave_add_buffer(current_wave,
			  buffer,
			  FALSE);

      phase = buffer->x % (guint) floor(AGS_FUNCTIONAL_PITCH_TEST_SAMPLERATE / AGS_FUNCTIONAL_PITCH_TEST_BASE_FREQ);
      ags_synth_util_sin(buffer->data,
			 AGS_FUNCTIONAL_PITCH_TEST_BASE_FREQ, phase, AGS_FUNCTIONAL_PITCH_TEST_VOLUME,
			 AGS_FUNCTIONAL_PITCH_TEST_SAMPLERATE, ags_audio_buffer_util_format_from_soundcard(AGS_FUNCTIONAL_PITCH_TEST_FORMAT),
			 0, AGS_FUNCTIONAL_PITCH_TEST_BUFFER_SIZE);
    }
  }
  
  start_list = NULL;
  
  for(i = (gint) AGS_FUNCTIONAL_PITCH_TEST_BASE_KEY; i <= (gint) AGS_FUNCTIONAL_PITCH_TEST_PITCH_UP_END_KEY; i++){
    struct AgsFunctionalPitchTestWave *test_wave;

    GList *pitch_wave;
    
    test_wave = ags_functional_pitch_test_alloc(start_wave);
    start_list = g_list_prepend(start_list,
				test_wave);

    pitch_wave = test_wave->wave;
    
    while(pitch_wave != NULL){
      GList *pitch_buffer;

      pitch_buffer = AGS_WAVE(pitch_wave->data)->buffer;
      
      for(k = 0; pitch_buffer != NULL; k++){	
	switch(AGS_FUNCTIONAL_PITCH_TEST_FORMAT){
	case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  ags_filter_util_pitch_s16(AGS_BUFFER(pitch_buffer->data)->data,
				    AGS_FUNCTIONAL_PITCH_TEST_BUFFER_SIZE,
				    AGS_FUNCTIONAL_PITCH_TEST_SAMPLERATE,
				    AGS_FUNCTIONAL_PITCH_TEST_BASE_KEY,
				    (gdouble) i * AGS_FUNCTIONAL_PITCH_TEST_TUNE);
	}
	break;
	}
	
	pitch_buffer = pitch_buffer->next;
      }

      pitch_wave = pitch_wave->next;
    }
  }

  start_list = g_list_reverse(start_list);

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(audio_application_context));

  list = start_list;
  success = TRUE;

  for(i = (gint) AGS_FUNCTIONAL_PITCH_TEST_BASE_KEY; i <= (gint) AGS_FUNCTIONAL_PITCH_TEST_PITCH_UP_END_KEY; i++){
    AgsStartAudio *start_audio;
    AgsStartSoundcard *start_soundcard;
    AgsCancelAudio *cancel_audio;

    struct AgsFunctionalPitchTestWave *test_wave;

    GList *task;

    g_message("playing key = %d", i);

    test_wave = list->data;
    
    wave_player->wave = test_wave->wave;

    /* start audio and soundcard task */
    task = NULL;    
    start_audio = ags_start_audio_new(wave_player,
				      AGS_SOUND_SCOPE_WAVE);
    task = g_list_prepend(task,
			  start_audio);
    
    start_soundcard = ags_start_soundcard_new();
    task = g_list_prepend(task,
			  start_soundcard);
    
    ags_task_launcher_add_task_all(task_launcher,
				   task);

    /* delay */
    usleep(AGS_FUNCTIONAL_PITCH_TEST_DELAY);
    
    /* create cancel task */
    cancel_audio = ags_cancel_audio_new(wave_player,
					AGS_SOUND_SCOPE_WAVE);
    
    /* append AgsCancelAudio */
    ags_task_launcher_add_task(task_launcher,
			       (AgsTask *) cancel_audio);

    usleep(1500000);
    
    /* iterate */
    list = list->next;
  }

  CU_ASSERT(success == TRUE);
}

void
ags_functional_pitch_test_pitch_down()
{
  AgsTaskLauncher *task_launcher;

  GList *start_wave, *wave;
  GList *start_list, *list;

  gint i, j, k;
  gboolean success;

  start_wave = NULL;
  
  for(i = 0; i < AGS_FUNCTIONAL_PITCH_TEST_AUDIO_CHANNELS; i++){
    AgsWave *current_wave;

    current_wave = ags_wave_new(wave_player,
				i);

    g_object_set(current_wave,
		 "samplerate", AGS_FUNCTIONAL_PITCH_TEST_SAMPLERATE,
		 "buffer-size", AGS_FUNCTIONAL_PITCH_TEST_BUFFER_SIZE,
		 "format", AGS_FUNCTIONAL_PITCH_TEST_FORMAT,
		 NULL);

    start_wave = ags_wave_add(start_wave,
			      current_wave);
    
    for(j = 0; j < floor(AGS_FUNCTIONAL_PITCH_TEST_PITCH_UP_FRAME_COUNT / AGS_FUNCTIONAL_PITCH_TEST_BUFFER_SIZE); j++){
      AgsBuffer *buffer;

      gdouble phase;

      buffer = ags_buffer_new();
      g_object_set(buffer,
		   "samplerate", AGS_FUNCTIONAL_PITCH_TEST_SAMPLERATE,
		   "buffer-size", AGS_FUNCTIONAL_PITCH_TEST_BUFFER_SIZE,
		   "format", AGS_FUNCTIONAL_PITCH_TEST_FORMAT,
		   "x", j * AGS_FUNCTIONAL_PITCH_TEST_BUFFER_SIZE,
		   NULL);
      ags_wave_add_buffer(current_wave,
			  buffer,
			  FALSE);

      phase = buffer->x % (guint) floor(AGS_FUNCTIONAL_PITCH_TEST_SAMPLERATE / AGS_FUNCTIONAL_PITCH_TEST_BASE_FREQ);
      ags_synth_util_sin(buffer->data,
			 AGS_FUNCTIONAL_PITCH_TEST_BASE_FREQ, phase, AGS_FUNCTIONAL_PITCH_TEST_VOLUME,
			 AGS_FUNCTIONAL_PITCH_TEST_SAMPLERATE, ags_audio_buffer_util_format_from_soundcard(AGS_FUNCTIONAL_PITCH_TEST_FORMAT),
			 0, AGS_FUNCTIONAL_PITCH_TEST_BUFFER_SIZE);
    }
  }
  
  start_list = NULL;
  
  for(i = (gint) AGS_FUNCTIONAL_PITCH_TEST_BASE_KEY; i >= (gint) AGS_FUNCTIONAL_PITCH_TEST_PITCH_DOWN_END_KEY; i--){
    struct AgsFunctionalPitchTestWave *test_wave;

    GList *pitch_wave;
    
    test_wave = ags_functional_pitch_test_alloc(start_wave);
    start_list = g_list_prepend(start_list,
				test_wave);

    pitch_wave = test_wave->wave;
    
    while(pitch_wave != NULL){
      GList *pitch_buffer;

      pitch_buffer = AGS_WAVE(pitch_wave->data)->buffer;
      
      for(k = 0; k < floor(AGS_FUNCTIONAL_PITCH_TEST_PITCH_UP_FRAME_COUNT / AGS_FUNCTIONAL_PITCH_TEST_BUFFER_SIZE); k++){
	switch(AGS_FUNCTIONAL_PITCH_TEST_FORMAT){
	case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  ags_filter_util_pitch_s16(AGS_BUFFER(pitch_buffer->data)->data,
				    AGS_FUNCTIONAL_PITCH_TEST_BUFFER_SIZE,
				    AGS_FUNCTIONAL_PITCH_TEST_SAMPLERATE,
				    AGS_FUNCTIONAL_PITCH_TEST_BASE_KEY,
				    (gdouble) i * AGS_FUNCTIONAL_PITCH_TEST_TUNE);
	}
	break;
	}
	
	pitch_buffer = pitch_buffer->next;
      }

      pitch_wave = pitch_wave->next;
    }
  }

  start_list = g_list_reverse(start_list);
  
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(audio_application_context));

  list = start_list;
  success = TRUE;

  for(i = (gint) AGS_FUNCTIONAL_PITCH_TEST_BASE_KEY; i >= (gint) AGS_FUNCTIONAL_PITCH_TEST_PITCH_DOWN_END_KEY; i--){
    AgsStartAudio *start_audio;
    AgsStartSoundcard *start_soundcard;
    AgsCancelAudio *cancel_audio;

    struct AgsFunctionalPitchTestWave *test_wave;

    GList *task;

    g_message("playing key = %d", i);

    test_wave = list->data;
    
    wave_player->wave = test_wave->wave;
    
    /* start audio and soundcard task */
    task = NULL;    
    start_audio = ags_start_audio_new(wave_player,
				      AGS_SOUND_SCOPE_WAVE);
    task = g_list_prepend(task,
			  start_audio);
    
    start_soundcard = ags_start_soundcard_new();
    task = g_list_prepend(task,
			  start_soundcard);
    
    ags_task_launcher_add_task_all(task_launcher,
				   task);

    /* delay */
    usleep(AGS_FUNCTIONAL_PITCH_TEST_DELAY);

    /* create cancel task */
    cancel_audio = ags_cancel_audio_new(wave_player,
					AGS_SOUND_SCOPE_WAVE);
    
    /* append AgsCancelAudio */
    ags_task_launcher_add_task(task_launcher,
			       (AgsTask *) cancel_audio);

    usleep(1500000);

    /* iterate */
    list = list->next;
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

