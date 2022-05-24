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
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

struct AgsFunctionalFastPitchTestWave
{
  gdouble freq;
  gdouble key;
  
  GList *wave;
};

gpointer ags_functional_fast_pitch_test_add_thread(gpointer data);

int ags_functional_fast_pitch_test_init_suite();
int ags_functional_fast_pitch_test_clean_suite();

void ags_functional_fast_pitch_test_pitch_up();
void ags_functional_fast_pitch_test_pitch_down();

struct AgsFunctionalFastPitchTestWave* ags_functional_fast_pitch_test_alloc(GList *template_wave);

#define AGS_FUNCTIONAL_FAST_PITCH_TEST_AUDIO_CHANNELS (2)

#define AGS_FUNCTIONAL_FAST_PITCH_TEST_SAMPLERATE (44100)
#define AGS_FUNCTIONAL_FAST_PITCH_TEST_BUFFER_SIZE (1024)
#define AGS_FUNCTIONAL_FAST_PITCH_TEST_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

#define AGS_FUNCTIONAL_FAST_PITCH_TEST_VOLUME (1.0)

#define AGS_FUNCTIONAL_FAST_PITCH_TEST_BASE_KEY (0.0)
#define AGS_FUNCTIONAL_FAST_PITCH_TEST_BASE_FREQ (440.0)

#define AGS_FUNCTIONAL_FAST_PITCH_TEST_TUNE (100.0)

#define AGS_FUNCTIONAL_FAST_PITCH_TEST_DELAY (8000000)

#define AGS_FUNCTIONAL_FAST_PITCH_TEST_PITCH_UP_END_KEY (48.0)
#define AGS_FUNCTIONAL_FAST_PITCH_TEST_PITCH_UP_FRAME_COUNT (5 * 44100)

#define AGS_FUNCTIONAL_FAST_PITCH_TEST_PITCH_DOWN_END_KEY (-48.0)
#define AGS_FUNCTIONAL_FAST_PITCH_TEST_PITCH_DOWN_FRAME_COUNT (5 * 44100)

#define AGS_FUNCTIONAL_FAST_PITCH_TEST_CONFIG "[generic]\n"	\
  "autosave-thread=false\n"					\
  "simple-file=true\n"						\
  "disable-feature=experimental\n"				\
  "segmentation=4/4\n"						\
  "\n"								\
  "[thread]\n"							\
  "model=super-threaded\n"					\
  "super-threaded-scope=audio\n"				\
  "lock-global=ags-thread\n"					\
  "lock-parent=ags-recycling-thread\n"				\
  "thread-pool-max-unused-threads=8\n"				\
  "max-precision=250\n"						\
  "\n"								\
  "[soundcard-0]\n"						\
  "backend=pulse\n"						\
  "device=ags-pulse-devout-0\n"					\
  "samplerate=44100\n"						\
  "buffer-size=1024\n"						\
  "pcm-channels=2\n"						\
  "dsp-channels=2\n"						\
  "format=16\n"							\
  "use-cache=true\n"						\
  "cache-buffer-size=4096\n"					\
  "\n"								\
  "[recall]\n"							\
  "auto-sense=true\n"						\
  "\n"

GThread *add_thread = NULL;

AgsAudioApplicationContext *audio_application_context;

AgsAudio *output_panel;
AgsAudio *wave_player;
AgsSynthUtil *synth_util;
AgsFastPitchUtil *fast_pitch_util;

GObject *output_soundcard;

gpointer
ags_functional_fast_pitch_test_add_thread(gpointer data)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C");
  putenv("LANG=C");

  putenv("LADSPA_PATH=\"\"");
  putenv("DSSI_PATH=\"\"");
  putenv("LV2_PATH=\"\"");
  putenv("VST3_PATH=\"\"");

  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    exit(CU_get_error());
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFunctionalFastPitchTest", ags_functional_fast_pitch_test_init_suite, ags_functional_fast_pitch_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    exit(CU_get_error());
  }
  
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_fast_pitch_util.h doing pitch up", ags_functional_fast_pitch_test_pitch_up) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fast_pitch_util.h doing pitch down", ags_functional_fast_pitch_test_pitch_down) == NULL)){
    CU_cleanup_registry();
      
    exit(CU_get_error());
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  exit(CU_get_error());
}

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_fast_pitch_test_init_suite()
{
  AgsPlaybackDomain *playback_domain;
  AgsChannel *channel, *link;
  AgsRecallContainer *playback_play_container, *playback_recall_container;
  AgsRecallContainer *buffer_play_container, *buffer_recall_container;  
  AgsAudioLoop *audio_loop;
  AgsAudioThread *audio_thread;

  AgsConfig *config;

  GList *start_list;

  gchar *str;
  
  guint i;
  
  GError *error;

  ags_priority_load_defaults(ags_priority_get_instance());  
  
  config = ags_config_get_instance();

#if defined(AGS_TEST_CONFIG)
  ags_config_load_from_data(config,
			    AGS_TEST_CONFIG,
			    strlen(AGS_TEST_CONFIG));
#else
  if((str = getenv("AGS_TEST_CONFIG")) != NULL){
    ags_config_load_from_data(config,
			      str,
			      strlen(str));
  }else{
    ags_config_load_from_data(config,
			      AGS_FUNCTIONAL_FAST_PITCH_TEST_CONFIG,
			      strlen(AGS_FUNCTIONAL_FAST_PITCH_TEST_CONFIG));
  }
#endif

  /* audio application context */
  audio_application_context = (AgsApplicationContext *) ags_audio_application_context_new();
  g_object_ref(audio_application_context);

  ags_application_context_prepare(audio_application_context);
  ags_application_context_setup(audio_application_context);

  /* audio loop */
  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(audio_application_context));

  /* output soundcard */
  output_soundcard = audio_application_context->soundcard->data;

  start_list = NULL;
  
  /* output panel */
  output_panel = ags_audio_new(output_soundcard);
  g_object_ref(output_panel);

  output_panel->flags |= AGS_AUDIO_CAN_NEXT_ACTIVE;
  
  start_list = g_list_prepend(start_list,
			      output_panel);
  g_object_ref(output_panel);
  
  ags_audio_set_flags(output_panel, (AGS_AUDIO_SYNC));

  /* ags-fx-playback */
  playback_play_container = ags_recall_container_new();
  playback_recall_container = ags_recall_container_new();
  
  ags_fx_factory_create(output_panel,
			playback_play_container, playback_recall_container,
			"ags-fx-playback",
			NULL,
			NULL,
			0, 0,
			0, 0,
			0,
			(AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
			0);

  /* set channels */
  ags_audio_set_audio_channels(output_panel,
			       AGS_FUNCTIONAL_FAST_PITCH_TEST_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(output_panel,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(output_panel,
		     AGS_TYPE_INPUT,
		     1, 0);

  /* ags-fx-playback */
  for(i = 0; i < AGS_FUNCTIONAL_FAST_PITCH_TEST_AUDIO_CHANNELS; i++){
    ags_fx_factory_create(output_panel,
			  playback_play_container, playback_recall_container,
			  "ags-fx-playback",
			  NULL,
			  NULL,
			  i, i + 1,
			  0, 1,
			  0,
			  (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT),
			  0);
  }

  ags_connectable_connect(AGS_CONNECTABLE(output_panel));

  /* wave player */
  wave_player = ags_audio_new(output_soundcard);
  g_object_ref(wave_player);
  
  wave_player->flags |= AGS_AUDIO_CAN_NEXT_ACTIVE;

  start_list = g_list_prepend(start_list,
			      wave_player);
  g_object_ref(wave_player);
    
  ags_audio_set_flags(wave_player, (AGS_AUDIO_SYNC |
				    AGS_AUDIO_OUTPUT_HAS_RECYCLING |
				    AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_ability_flags(wave_player, (AGS_SOUND_ABILITY_NOTATION));
  
  /* ags-fx */
  playback_play_container = ags_recall_container_new();
  playback_recall_container = ags_recall_container_new();

  ags_fx_factory_create(wave_player,
			playback_play_container, playback_recall_container,
			"ags-fx-playback",
			NULL,
			NULL,
			0, 0,
			0, 0,
			0,
			(AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
			0);

  buffer_play_container = ags_recall_container_new();
  buffer_recall_container = ags_recall_container_new();

  ags_fx_factory_create(wave_player,
			buffer_play_container, buffer_recall_container,
			"ags-fx-buffer",
			NULL,
			NULL,
			0, 0,
			0, 0,
			0,
			(AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
			0);

  ags_audio_set_audio_channels(wave_player,
			       AGS_FUNCTIONAL_FAST_PITCH_TEST_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(wave_player,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(wave_player,
		     AGS_TYPE_INPUT,
		     1, 0);

  playback_domain = NULL;
  
  g_object_get(wave_player,
	       "playback-domain", &playback_domain,
	       NULL);
  
  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
						      AGS_SOUND_SCOPE_NOTATION);
	
  ags_thread_add_start_queue(audio_loop,
			     audio_thread);

  channel = wave_player->output;
  
  for(i = 0; i < AGS_FUNCTIONAL_FAST_PITCH_TEST_AUDIO_CHANNELS; i++){
    AgsPlayback *playback;
	
    AgsChannelThread *channel_thread;

    playback = NULL;

    g_object_get(channel,
		 "playback", &playback,
		 NULL);

    channel_thread = ags_playback_get_channel_thread(playback,
						     AGS_SOUND_SCOPE_NOTATION);
	
    ags_thread_add_start_queue(audio_loop,
			       channel_thread);

    channel = channel->next;
  }
  
  /* ags-fx */
  for(i = 0; i < AGS_FUNCTIONAL_FAST_PITCH_TEST_AUDIO_CHANNELS; i++){
    ags_fx_factory_create(wave_player,
			  playback_play_container, playback_recall_container,
			  "ags-fx-playback",
			  NULL,
			  NULL,
			  i, i + 1,
			  0, 1,
			  0,
			  (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT),
			  0);

    ags_fx_factory_create(wave_player,
			  buffer_play_container, buffer_recall_container,
			  "ags-fx-buffer",
			  NULL,
			  NULL,
			  i, i + 1,
			  0, 1,
			  0,
			  (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT),
			  0);
  }
  
  ags_connectable_connect(AGS_CONNECTABLE(wave_player));

  /*  */
  start_list = g_list_reverse(start_list);
  ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(audio_application_context),
			       start_list);

  /* fast pitch util */
  synth_util = ags_synth_util_alloc();

  fast_pitch_util = ags_fast_pitch_util_alloc();
  
  /* link */
  channel = output_panel->input;
  link = wave_player->output;
  
  for(i = 0; i < AGS_FUNCTIONAL_FAST_PITCH_TEST_AUDIO_CHANNELS; i++){
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
ags_functional_fast_pitch_test_clean_suite()
{
  g_object_unref(audio_application_context);
  
  return(0);
}

struct AgsFunctionalFastPitchTestWave*
ags_functional_fast_pitch_test_alloc(GList *template_wave)
{
  struct AgsFunctionalFastPitchTestWave *test_wave;

  guint i_stop, j_stop;
  guint i, j;
  
  test_wave = (struct AgsFunctionalFastPitchTestWave *) malloc(sizeof(struct AgsFunctionalFastPitchTestWave));

  test_wave->key = AGS_FUNCTIONAL_FAST_PITCH_TEST_BASE_KEY;
  test_wave->freq = AGS_FUNCTIONAL_FAST_PITCH_TEST_BASE_FREQ;

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
				 ags_timestamp_get_ags_offset(current_wave->timestamp));
    
    g_object_set(wave,
		 "samplerate", AGS_FUNCTIONAL_FAST_PITCH_TEST_SAMPLERATE,
		 "buffer-size", AGS_FUNCTIONAL_FAST_PITCH_TEST_BUFFER_SIZE,
		 "format", AGS_FUNCTIONAL_FAST_PITCH_TEST_FORMAT,
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
ags_functional_fast_pitch_test_pitch_up()
{
  AgsTaskLauncher *task_launcher;
  
  GList *start_wave, *wave;
  GList *start_list, *list;

  gint i, j, k;
  gboolean success;
  
  ags_synth_util_set_buffer_length(synth_util,
				   AGS_FUNCTIONAL_FAST_PITCH_TEST_BUFFER_SIZE);
  ags_synth_util_set_format(synth_util,
			    AGS_SOUNDCARD_SIGNED_16_BIT);
  ags_synth_util_set_samplerate(synth_util,
				AGS_FUNCTIONAL_FAST_PITCH_TEST_SAMPLERATE);

  ags_synth_util_set_synth_oscillator_mode(synth_util,
					   AGS_SYNTH_OSCILLATOR_SIN);

  ags_synth_util_set_frequency(synth_util,
			       AGS_FUNCTIONAL_FAST_PITCH_TEST_BASE_FREQ);

  ags_synth_util_set_volume(synth_util,
			    AGS_FUNCTIONAL_FAST_PITCH_TEST_VOLUME);
  
  start_wave = NULL;
  
  for(i = 0; i < AGS_FUNCTIONAL_FAST_PITCH_TEST_AUDIO_CHANNELS; i++){
    AgsWave *current_wave;
    
    current_wave = ags_wave_new(wave_player,
				i);

    g_object_set(current_wave,
		 "samplerate", AGS_FUNCTIONAL_FAST_PITCH_TEST_SAMPLERATE,
		 "buffer-size", AGS_FUNCTIONAL_FAST_PITCH_TEST_BUFFER_SIZE,
		 "format", AGS_FUNCTIONAL_FAST_PITCH_TEST_FORMAT,
		 NULL);

    ags_audio_add_wave(wave_player,
    		       current_wave);

    start_wave = ags_wave_add(start_wave,
			      current_wave);
    
    for(j = 0; j < floor(AGS_FUNCTIONAL_FAST_PITCH_TEST_PITCH_UP_FRAME_COUNT / AGS_FUNCTIONAL_FAST_PITCH_TEST_BUFFER_SIZE); j++){
      AgsBuffer *buffer;

      gdouble phase;
      
      buffer = ags_buffer_new();
      g_object_set(buffer,
		   "samplerate", AGS_FUNCTIONAL_FAST_PITCH_TEST_SAMPLERATE,
		   "buffer-size", AGS_FUNCTIONAL_FAST_PITCH_TEST_BUFFER_SIZE,
		   "format", AGS_FUNCTIONAL_FAST_PITCH_TEST_FORMAT,
		   "x", (guint64) (j * AGS_FUNCTIONAL_FAST_PITCH_TEST_BUFFER_SIZE),
		   NULL);
      ags_wave_add_buffer(current_wave,
			  buffer,
			  FALSE);
      
      phase = buffer->x % (guint) floor(AGS_FUNCTIONAL_FAST_PITCH_TEST_SAMPLERATE / AGS_FUNCTIONAL_FAST_PITCH_TEST_BASE_FREQ);

      ags_synth_util_set_source(synth_util,
				buffer->data);

      ags_synth_util_set_phase(synth_util,
			       phase);
      
      ags_synth_util_compute_sin(synth_util);
    }
  }
    
  ags_fast_pitch_util_set_buffer_length(fast_pitch_util,
					AGS_FUNCTIONAL_FAST_PITCH_TEST_BUFFER_SIZE);
  ags_fast_pitch_util_set_format(fast_pitch_util,
				 AGS_SOUNDCARD_SIGNED_16_BIT);
  ags_fast_pitch_util_set_samplerate(fast_pitch_util,
				     AGS_FUNCTIONAL_FAST_PITCH_TEST_SAMPLERATE);

  ags_fast_pitch_util_set_base_key(fast_pitch_util,
				   AGS_FUNCTIONAL_FAST_PITCH_TEST_BASE_KEY);

  start_list = NULL;

  for(i = (gint) AGS_FUNCTIONAL_FAST_PITCH_TEST_BASE_KEY; i <= (gint) AGS_FUNCTIONAL_FAST_PITCH_TEST_PITCH_UP_END_KEY; i++){
    struct AgsFunctionalFastPitchTestWave *test_wave;

    GList *pitch_wave;
    
    test_wave = ags_functional_fast_pitch_test_alloc(start_wave);
    start_list = g_list_prepend(start_list,
				test_wave);

    pitch_wave = test_wave->wave;
    
    while(pitch_wave != NULL){
      GList *pitch_buffer;

      pitch_buffer = AGS_WAVE(pitch_wave->data)->buffer;

      ags_fast_pitch_util_set_tuning(fast_pitch_util,
				     i * AGS_FUNCTIONAL_FAST_PITCH_TEST_TUNE);
      
      for(k = 0; pitch_buffer != NULL; k++){
	ags_fast_pitch_util_set_source(fast_pitch_util,
				       AGS_BUFFER(g_list_nth_data(AGS_WAVE(start_wave->data)->buffer, k))->data);

	ags_fast_pitch_util_set_destination(fast_pitch_util,
					    AGS_BUFFER(pitch_buffer->data)->data);
	  
	ags_fast_pitch_util_pitch(fast_pitch_util);
	
	pitch_buffer = pitch_buffer->next;
      }

      pitch_wave = pitch_wave->next;
    }
  }

  start_list = g_list_reverse(start_list);

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(audio_application_context));

  list = start_list;
  success = TRUE;

  for(i = (gint) AGS_FUNCTIONAL_FAST_PITCH_TEST_BASE_KEY; i <= (gint) AGS_FUNCTIONAL_FAST_PITCH_TEST_PITCH_UP_END_KEY; i++){
    AgsStartAudio *start_audio;
    AgsStartSoundcard *start_soundcard;
    AgsCancelAudio *cancel_audio;

    struct AgsFunctionalFastPitchTestWave *test_wave;

    GList *task;

    g_message("playing key = %d", i);

    test_wave = list->data;
    
    wave_player->wave = test_wave->wave;

    /* start audio and soundcard task */
    task = NULL;    
    start_audio = ags_start_audio_new(wave_player,
				      AGS_SOUND_SCOPE_NOTATION);
    task = g_list_prepend(task,
			  start_audio);
    
    start_soundcard = ags_start_soundcard_new();
    task = g_list_prepend(task,
			  start_soundcard);
    
    ags_task_launcher_add_task_all(task_launcher,
				   task);
    
    /* delay */
    usleep(AGS_FUNCTIONAL_FAST_PITCH_TEST_DELAY);
    
    /* create cancel task */
    cancel_audio = ags_cancel_audio_new(wave_player,
					AGS_SOUND_SCOPE_NOTATION);
    
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
ags_functional_fast_pitch_test_pitch_down()
{
  AgsTaskLauncher *task_launcher;

  GList *start_wave, *wave;
  GList *start_list, *list;

  gint i, j, k;
  gboolean success;

  ags_synth_util_set_buffer_length(synth_util,
				   AGS_FUNCTIONAL_FAST_PITCH_TEST_BUFFER_SIZE);
  ags_synth_util_set_format(synth_util,
			    AGS_SOUNDCARD_SIGNED_16_BIT);
  ags_synth_util_set_samplerate(synth_util,
				AGS_FUNCTIONAL_FAST_PITCH_TEST_SAMPLERATE);

  ags_synth_util_set_synth_oscillator_mode(synth_util,
					   AGS_SYNTH_OSCILLATOR_SIN);

  ags_synth_util_set_frequency(synth_util,
			       AGS_FUNCTIONAL_FAST_PITCH_TEST_BASE_FREQ);

  ags_synth_util_set_volume(synth_util,
			    AGS_FUNCTIONAL_FAST_PITCH_TEST_VOLUME);

  start_wave = NULL;
  
  for(i = 0; i < AGS_FUNCTIONAL_FAST_PITCH_TEST_AUDIO_CHANNELS; i++){
    AgsWave *current_wave;

    current_wave = ags_wave_new(wave_player,
				i);

    g_object_set(current_wave,
		 "samplerate", AGS_FUNCTIONAL_FAST_PITCH_TEST_SAMPLERATE,
		 "buffer-size", AGS_FUNCTIONAL_FAST_PITCH_TEST_BUFFER_SIZE,
		 "format", AGS_FUNCTIONAL_FAST_PITCH_TEST_FORMAT,
		 NULL);

    start_wave = ags_wave_add(start_wave,
			      current_wave);
    
    for(j = 0; j < floor(AGS_FUNCTIONAL_FAST_PITCH_TEST_PITCH_UP_FRAME_COUNT / AGS_FUNCTIONAL_FAST_PITCH_TEST_BUFFER_SIZE); j++){
      AgsBuffer *buffer;

      gdouble phase;

      buffer = ags_buffer_new();
      g_object_set(buffer,
		   "samplerate", AGS_FUNCTIONAL_FAST_PITCH_TEST_SAMPLERATE,
		   "buffer-size", AGS_FUNCTIONAL_FAST_PITCH_TEST_BUFFER_SIZE,
		   "format", AGS_FUNCTIONAL_FAST_PITCH_TEST_FORMAT,
		   "x", (guint64) (j * AGS_FUNCTIONAL_FAST_PITCH_TEST_BUFFER_SIZE),
		   NULL);
      ags_wave_add_buffer(current_wave,
			  buffer,
			  FALSE);

      phase = buffer->x % (guint) floor(AGS_FUNCTIONAL_FAST_PITCH_TEST_SAMPLERATE / AGS_FUNCTIONAL_FAST_PITCH_TEST_BASE_FREQ);

      ags_synth_util_set_source(synth_util,
				buffer->data);

      ags_synth_util_set_phase(synth_util,
			       phase);
      
      ags_synth_util_compute_sin(synth_util);
    }
  }

  ags_fast_pitch_util_set_buffer_length(fast_pitch_util,
					AGS_FUNCTIONAL_FAST_PITCH_TEST_BUFFER_SIZE);
  ags_fast_pitch_util_set_format(fast_pitch_util,
				 AGS_SOUNDCARD_SIGNED_16_BIT);
  ags_fast_pitch_util_set_samplerate(fast_pitch_util,
				     AGS_FUNCTIONAL_FAST_PITCH_TEST_SAMPLERATE);

  ags_fast_pitch_util_set_base_key(fast_pitch_util,
				   AGS_FUNCTIONAL_FAST_PITCH_TEST_BASE_KEY);
  
  start_list = NULL;
  
  for(i = (gint) AGS_FUNCTIONAL_FAST_PITCH_TEST_BASE_KEY; i >= (gint) AGS_FUNCTIONAL_FAST_PITCH_TEST_PITCH_DOWN_END_KEY; i--){
    struct AgsFunctionalFastPitchTestWave *test_wave;

    GList *pitch_wave;
    
    test_wave = ags_functional_fast_pitch_test_alloc(start_wave);
    start_list = g_list_prepend(start_list,
				test_wave);

    pitch_wave = test_wave->wave;
    
    while(pitch_wave != NULL){
      GList *pitch_buffer;

      pitch_buffer = AGS_WAVE(pitch_wave->data)->buffer;

      ags_fast_pitch_util_set_tuning(fast_pitch_util,
				     i * AGS_FUNCTIONAL_FAST_PITCH_TEST_TUNE);	  
      
      for(k = 0; k < floor(AGS_FUNCTIONAL_FAST_PITCH_TEST_PITCH_UP_FRAME_COUNT / AGS_FUNCTIONAL_FAST_PITCH_TEST_BUFFER_SIZE); k++){
	ags_fast_pitch_util_set_source(fast_pitch_util,
				       AGS_BUFFER(g_list_nth_data(AGS_WAVE(start_wave->data)->buffer, k))->data);

	ags_fast_pitch_util_set_destination(fast_pitch_util,
					    AGS_BUFFER(pitch_buffer->data)->data);
	  
	ags_fast_pitch_util_pitch(fast_pitch_util);
	
	pitch_buffer = pitch_buffer->next;
      }

      pitch_wave = pitch_wave->next;
    }
  }

  start_list = g_list_reverse(start_list);
  
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(audio_application_context));

  list = start_list;
  success = TRUE;

  for(i = (gint) AGS_FUNCTIONAL_FAST_PITCH_TEST_BASE_KEY; i >= (gint) AGS_FUNCTIONAL_FAST_PITCH_TEST_PITCH_DOWN_END_KEY; i--){
    AgsStartAudio *start_audio;
    AgsStartSoundcard *start_soundcard;
    AgsCancelAudio *cancel_audio;

    struct AgsFunctionalFastPitchTestWave *test_wave;

    GList *task;

    g_message("playing key = %d", i);

    test_wave = list->data;
    
    wave_player->wave = test_wave->wave;
    
    /* start audio and soundcard task */
    task = NULL;    
    start_audio = ags_start_audio_new(wave_player,
				      AGS_SOUND_SCOPE_NOTATION);
    task = g_list_prepend(task,
			  start_audio);
    
    start_soundcard = ags_start_soundcard_new();
    task = g_list_prepend(task,
			  start_soundcard);
    
    ags_task_launcher_add_task_all(task_launcher,
				   task);

    /* delay */
    usleep(AGS_FUNCTIONAL_FAST_PITCH_TEST_DELAY);

    /* create cancel task */
    cancel_audio = ags_cancel_audio_new(wave_player,
					AGS_SOUND_SCOPE_NOTATION);
    
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
  add_thread = g_thread_new("libags_audio.so - functional fast pitch test",
			    ags_functional_fast_pitch_test_add_thread,
			    NULL);

  g_main_loop_run(g_main_loop_new(g_main_context_default(),
				  FALSE));
  
  g_thread_join(add_thread);
  
  return(-1);
}
