/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2026 Joël Krähemann
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

int ags_functional_frame_clock_test_init_suite();
int ags_functional_frame_clock_test_clean_suite();

gpointer ags_functional_frame_clock_test_add_thread(gpointer data);

void ags_functional_frame_clock_test_playback_add_sink(AgsAudio *audio);
void ags_functional_frame_clock_test_playback_add_mixer(AgsAudio *audio);
void ags_functional_frame_clock_test_playback_add_playback(AgsAudio *audio);
void ags_functional_frame_clock_test_playback_start_audio(AgsAudio *audio,
							  AgsTaskLauncher *task_launcher);
void ags_functional_frame_clock_test_playback_stop_audio(AgsAudio *audio,
							 AgsTaskLauncher *task_launcher,
							 struct timespec *start_time);


void ags_functional_frame_clock_test_counter_increment();

void tic_callback(GObject *soundcard, gpointer user_data);

void activate(GApplication *app,
	      gpointer user_data);

#define AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO_CHANNELS (2)
#define AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_PADS (128)
#define AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO (1)

#define AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_BUFFER (5)

#define AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_NOTES (4 * (guint) (1.0 / AGS_NOTATION_MINIMUM_NOTE_LENGTH) * 120)
#define AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_STOP_DELAY (120)

 // Apple Inc. - Built-in Output
#define AGS_FUNCTIONAL_FRAME_CLOCK_TEST_CONFIG "[generic]\n"	\
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
  "max-precision=60\n"						\
  "\n"								\
  "[soundcard]\n"						\
  "backend=core-audio\n"					\
  "capability=playback\n"					\
  "device=Apple Inc. - MacBook Pro-Lautsprecher\n"		\
  "samplerate=44100\n"						\
  "buffer-size=2048\n"						\
  "pcm-channels=2\n"						\
  "dsp-channels=2\n"						\
  "format=float\n"						\
  "\n"								\
  "[recall]\n"							\
  "auto-sense=true\n"						\
  "\n"

GThread *add_thread = NULL;

AgsAudioApplicationContext *audio_application_context;

GObject *output_soundcard = NULL;

gpointer
ags_functional_frame_clock_test_add_thread(gpointer data)
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
  pSuite = CU_add_suite("AgsFunctionalFrameClockTest", ags_functional_frame_clock_test_init_suite, ags_functional_frame_clock_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    exit(CU_get_error());
  }
  
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsFrameClock increment counter", ags_functional_frame_clock_test_counter_increment) == NULL)){
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
ags_functional_frame_clock_test_init_suite()
{
  AgsAudioLoop *audio_loop;
  
  AgsConfig *config;

  gchar *str;
  
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
			      AGS_FUNCTIONAL_FRAME_CLOCK_TEST_CONFIG,
			      strlen(AGS_FUNCTIONAL_FRAME_CLOCK_TEST_CONFIG));
  }
#endif

  audio_application_context = (AgsApplicationContext *) ags_audio_application_context_new();
  g_object_ref(audio_application_context);

  ags_application_context_prepare(audio_application_context);
  ags_application_context_setup(audio_application_context);

  ags_connectable_connect(AGS_CONNECTABLE(audio_application_context));
  
  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(audio_application_context));

  output_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(audio_application_context));

  g_signal_connect(output_soundcard, "tic",
		   G_CALLBACK(tic_callback), NULL);
    
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_frame_clock_test_clean_suite()
{
  g_object_unref(audio_application_context);
  
  return(0);
}

void
ags_functional_frame_clock_test_playback_add_sink(AgsAudio *audio)
{
  AgsRecallContainer *playback_play_container, *playback_recall_container;

  guint i;

  g_object_set(audio,
	       "output-soundcard", output_soundcard,
	       NULL);
  
  /* ags-fx-playback */
  playback_play_container = ags_recall_container_new();
  playback_recall_container = ags_recall_container_new();
  
  ags_fx_factory_create(audio,
			playback_play_container, playback_recall_container,
			"ags-fx-playback",
			NULL,
			NULL,
			0, 0,
			0, 0,
			0,
			(AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
			0);
  
  ags_audio_set_audio_channels(audio,
			       AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     1, 0);

  for(i = 0; i < AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO_CHANNELS; i++){
    ags_fx_factory_create(audio,
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
}
  
void
ags_functional_frame_clock_test_playback_add_mixer(AgsAudio *audio)
{
  AgsRecallContainer *volume_play_container, *volume_recall_container;

  guint i, j;
  
  g_object_set(audio,
	       "output-soundcard", output_soundcard,
	       NULL);
  
  /* ags-fx-volume */
  volume_play_container = ags_recall_container_new();
  volume_recall_container = ags_recall_container_new();
  
  ags_fx_factory_create(audio,
			volume_play_container, volume_recall_container,
			"ags-fx-volume",
			NULL,
			NULL,
			0, 0,
			0, 0,
			0,
			(AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
			0);
  
  ags_audio_set_audio_channels(audio,
			       AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO, 0);
  
  for(i = 0; i < AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO_CHANNELS; i++){
    for(j = 0; j < AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO; j++){
      ags_fx_factory_create(audio,
			    volume_play_container, volume_recall_container,
			    "ags-fx-volume",
			    NULL,
			    NULL,
			    i, i + 1,
			    j, j + 1,
			    0,
			    (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT),
			    0);
    }
  }
}
  
void
ags_functional_frame_clock_test_playback_add_playback(AgsAudio *audio)
{
  AgsChannel *channel, *link;
  AgsPlaybackDomain *playback_domain;
  AgsRecallContainer *notation_play_container, *notation_recall_container;
  AgsRecallContainer *buffer_play_container, *buffer_recall_container;

  AgsAudioLoop *audio_loop;
  AgsAudioThread *audio_thread;

  guint i;
  
  g_object_set(audio,
	       "output-soundcard", output_soundcard,
	       NULL);
  
  /* audio loop */
  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(audio_application_context));

  ags_audio_set_audio_channels(audio,
			       AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_PADS, 0);
    
  /* ags-fx-notation */
  notation_play_container = ags_recall_container_new();
  notation_recall_container = ags_recall_container_new();

  ags_fx_factory_create(audio,
			notation_play_container, notation_recall_container,
			"ags-fx-notation",
			NULL,
			NULL,
			0, AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO_CHANNELS,
			0, AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_PADS,
			0,
			(AGS_FX_FACTORY_INPUT | AGS_FX_FACTORY_ADD),
			0);

  /* ags-fx-buffer */
  buffer_play_container = ags_recall_container_new();
  buffer_recall_container = ags_recall_container_new();

  ags_fx_factory_create(audio,
			buffer_play_container, buffer_recall_container,
			"ags-fx-buffer",
			NULL,
			NULL,
			0, AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO_CHANNELS,
			0, AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_PADS,
			0,
			(AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
			0);

  /*  */
#if 0
  playback_domain = NULL;
  
  g_object_get(audio,
	       "playback-domain", &playback_domain,
	       NULL);
  
  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
						      AGS_SOUND_SCOPE_NOTATION);
	
  ags_thread_add_start_queue(audio_loop,
			     audio_thread);

  channel = audio->output;
  
  for(i = 0; i < AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO_CHANNELS; i++){
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
#endif
}

void
ags_functional_frame_clock_test_playback_start_audio(AgsAudio *audio,
						     AgsTaskLauncher *task_launcher)
{
  AgsStartAudio *start_audio;
  AgsStartSoundcard *start_soundcard;

  GList *task;

  task = NULL;    
  start_audio = ags_start_audio_new(audio,
				    AGS_SOUND_SCOPE_NOTATION);
  task = g_list_prepend(task,
			start_audio);
    
  start_soundcard = ags_start_soundcard_new();
  task = g_list_prepend(task,
			start_soundcard);

  /* append AgsStartSoundcard */
  // task = g_list_reverse(task);
      
  ags_task_launcher_add_task_all(task_launcher,
				 task);
}
  
void
ags_functional_frame_clock_test_playback_stop_audio(AgsAudio *audio,
						    AgsTaskLauncher *task_launcher,
						    struct timespec *start_time)
{
  AgsCancelAudio *cancel_audio;

  AgsFrameClock *frame_clock;

  struct timespec sleep_time;
    
  clock_gettime(CLOCK_MONOTONIC, &sleep_time);
  
  while(sleep_time.tv_sec < start_time->tv_sec + AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_STOP_DELAY){
    usleep(AGS_USEC_PER_SEC);
    clock_gettime(CLOCK_MONOTONIC, &sleep_time);

    g_message("ags_functional_frame_clock_test_playback_stop_audio() - usleep %ds [%x]",
	      start_time->tv_sec + AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_STOP_DELAY - sleep_time.tv_sec,
	      audio);
  }
    
  /* create cancel task */
  cancel_audio = ags_cancel_audio_new(audio,
				      AGS_SOUND_SCOPE_NOTATION);
    
  /* append AgsCancelAudio */
  ags_task_launcher_add_task(task_launcher,
			     (AgsTask *) cancel_audio);
}

void
ags_functional_frame_clock_test_counter_increment()
{
  AgsAudio *panel, *mixer;
  AgsAudio **audio;
  AgsChannel *channel, *link, *current;

  AgsTaskLauncher *task_launcher;

  AgsThread *audio_loop, *soundcard_thread;
  
  GObject *soundcard;

  GList *start_queue;
  GList *list;

  struct timespec start_time;
  
  guint i, j, k;
  
  GError *error;

  /*
   * Setup audio tree sink, mixer and notation player as source
   */
  soundcard = NULL;
  
  if(audio_application_context->soundcard != NULL){
    soundcard = audio_application_context->soundcard->data;
  }

  CU_ASSERT(soundcard != NULL);
  
  /*
   * Start threads and enable playback
   */
  audio_loop = AGS_APPLICATION_CONTEXT(audio_application_context)->main_loop;
  
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(audio_application_context));
  soundcard_thread = ags_thread_find_type(audio_loop,
					  AGS_TYPE_SOUNDCARD_THREAD);

  CU_ASSERT(audio_loop != NULL);
  CU_ASSERT(soundcard_thread != NULL);
  
  CU_ASSERT(task_launcher != NULL);

  /* wait for audio loop */
  g_mutex_lock(AGS_THREAD_GET_START_MUTEX(audio_loop));

  if(ags_thread_test_status_flags(audio_loop, AGS_THREAD_STATUS_START_WAIT)){
    ags_thread_unset_status_flags(audio_loop, AGS_THREAD_STATUS_START_DONE);
      
    while(ags_thread_test_status_flags(audio_loop, AGS_THREAD_STATUS_START_WAIT) &&
	  !ags_thread_test_status_flags(audio_loop, AGS_THREAD_STATUS_START_DONE)){
      g_cond_wait(AGS_THREAD_GET_START_COND(audio_loop),
		  AGS_THREAD_GET_START_MUTEX(audio_loop));
    }
  }

  g_mutex_unlock(AGS_THREAD_GET_START_MUTEX(audio_loop));

  /* the output panel */
  panel = ags_audio_new(soundcard);
  g_object_ref(panel);
  
  panel->flags |= AGS_AUDIO_CAN_NEXT_ACTIVE;
  
  ags_audio_set_flags(panel, (AGS_AUDIO_SYNC));

  list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(audio_application_context));
  ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(audio_application_context),
			       g_list_prepend(list,
					      panel));
  
  ags_functional_frame_clock_test_playback_add_sink(panel);

  ags_connectable_connect(AGS_CONNECTABLE(panel));
  
  /* assert recycling NULL */
  channel = panel->output;
  
  for(i = 0; i < AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO_CHANNELS; i++){
    CU_ASSERT(channel->first_recycling == NULL);
    CU_ASSERT(channel->last_recycling == NULL);
    
    channel = channel->next;
  }

  channel = panel->input;
  
  for(i = 0; i < AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO_CHANNELS; i++){
    CU_ASSERT(channel->first_recycling == NULL);
    CU_ASSERT(channel->last_recycling == NULL);
    
    channel = channel->next;
  }

  /* the mixer */
  mixer = ags_audio_new(soundcard);
  g_object_ref(mixer);
  
  ags_audio_set_flags(panel, (AGS_AUDIO_ASYNC));

  list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(audio_application_context));
  ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(audio_application_context),
			       g_list_prepend(list,
					      mixer));
  
  ags_functional_frame_clock_test_playback_add_mixer(mixer);

  ags_connectable_connect(AGS_CONNECTABLE(mixer));

  /* set link, assert link set and recycling NULL */
  channel = mixer->output;
  link = panel->input;
  
  for(i = 0; i < AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO_CHANNELS; i++){
    error = NULL;
    ags_channel_set_link(channel,
			 link,
			 &error);

    CU_ASSERT(error == NULL);

    CU_ASSERT(channel->link == link);
    CU_ASSERT(link->link == channel);
    
    CU_ASSERT(channel->first_recycling == NULL);
    CU_ASSERT(channel->last_recycling == NULL);

    CU_ASSERT(link->first_recycling == NULL);
    CU_ASSERT(link->last_recycling == NULL);

    channel = channel->next;
    link = link->next;
  }

  /* create sources */
  audio = (AgsAudio **) malloc(AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO * sizeof(AgsAudio *));

  for(i = 0; i < AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO; i++){
    AgsNotation *notation;

    AgsTimestamp *timestamp;

    GList *list_notation;
    
    audio[i] = ags_audio_new(soundcard);
    g_object_ref(audio[i]);
    ags_audio_set_flags(audio[i], (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
				   AGS_AUDIO_INPUT_HAS_RECYCLING |
				   AGS_AUDIO_SYNC |
				   AGS_AUDIO_ASYNC));
    ags_audio_set_ability_flags(audio[i], (AGS_SOUND_ABILITY_PLAYBACK | AGS_SOUND_ABILITY_SEQUENCER | AGS_SOUND_ABILITY_NOTATION));
    ags_audio_set_behaviour_flags(audio[i], (AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT));
  
    g_object_set(audio[i],
		 "min-audio-channels", 1,
		 "min-output-pads", 1,
		 "min-input-pads", 1,
		 "max-input-pads", 128,
		 "audio-start-mapping", 0,
		 "audio-end-mapping", 128,
		 "midi-start-mapping", 0,
		 "midi-end-mapping", 128,
		 NULL);
    
    list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(audio_application_context));
    ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(audio_application_context),
				 g_list_prepend(list,
						audio[i]));
    
    ags_functional_frame_clock_test_playback_add_playback(audio[i]);

    ags_connectable_connect(AGS_CONNECTABLE(audio[i]));
    
    /* populate notation and set link, assert link set and recycling not NULL */
    channel = audio[i]->output;
    link = ags_channel_pad_nth(mixer->input,
			       i);
  
    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;

    for(j = 0; j < AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO_CHANNELS; j++){
      AgsAudioSignal *destination;

      destination = ags_audio_signal_new_with_length(soundcard,
						     channel->first_recycling,
						     NULL,
						     AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_BUFFER);
      destination->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
      ags_recycling_add_audio_signal(channel->first_recycling,
				     destination);
      
      /* populate notation */
      for(k = 0; k < AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_NOTES; k++){
	AgsNote *note;

	note = ags_note_new();
	note->x[0] = k + (rand() % 4);
	note->x[1] = note->x[0] + (rand() % 3) + 1;
	note->y = rand() % AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_PADS;

	timestamp->timer.ags_offset.offset = (guint64) AGS_NOTATION_DEFAULT_OFFSET * floor((double) note->x[0] / (double) AGS_NOTATION_DEFAULT_OFFSET);

	list_notation = audio[i]->notation;
	list_notation = ags_notation_find_near_timestamp(list_notation, j,
							 timestamp);

	if(list_notation != NULL){
	  notation = list_notation->data;
	}else{
	  notation = ags_notation_new(audio[i],
				      j);
	  notation->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;
	  
	  audio[i]->notation = ags_notation_add(audio[i]->notation,
						notation);
	}
	
	ags_notation_add_note(notation,
			      note,
			      FALSE);
      }
      
      /* set link */
      error = NULL;
      ags_channel_set_link(channel,
			   link,
			   &error);

      CU_ASSERT(error == NULL);

      CU_ASSERT(channel->link == link);
      CU_ASSERT(link->link == channel);
    
      CU_ASSERT(channel->first_recycling != NULL);
      CU_ASSERT(channel->last_recycling != NULL);

      CU_ASSERT(link->first_recycling == channel->first_recycling);
      CU_ASSERT(link->last_recycling == channel->last_recycling);

      channel = channel->next;
      link = link->next;
    }

    channel = audio[i]->input;
    
    for(j = 0; j < AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_PADS; j++){
      AgsAudioSignal *source;

      source = ags_audio_signal_new_with_length(soundcard,
						channel->first_recycling,
						NULL,
						AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_BUFFER);
      source->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
      ags_recycling_add_audio_signal(channel->first_recycling,
				     source);

      channel = channel->next;
    }
    
    /* find recycling within mixer */
    channel = audio[i]->output;
    current = mixer->output;

    for(j = 0; j < AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO_CHANNELS; j++){
      CU_ASSERT(ags_recycling_position(current->first_recycling, current->last_recycling->next,
				       channel->first_recycling) != -1);

      channel = channel->next;
      current = current->next;
    }

    /* find recycling within panel */
    channel = audio[i]->output;
    current = panel->input;

    for(j = 0; j < AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO_CHANNELS; j++){
      CU_ASSERT(ags_recycling_position(current->first_recycling, current->last_recycling->next,
				       channel->first_recycling) != -1);

      channel = channel->next;
      current = current->next;
    }
  }
  
  /* start playback */
  g_message("start playback");
  
  for(i = 0; i < AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO; i++){
    ags_functional_frame_clock_test_playback_start_audio(audio[i],
							 task_launcher);
  }

  clock_gettime(CLOCK_MONOTONIC, &start_time);

  /* stop playback */
  for(i = 0; i < AGS_FUNCTIONAL_FRAME_CLOCK_TEST_PLAYBACK_N_AUDIO; i++){
    ags_functional_frame_clock_test_playback_stop_audio(audio[i],
							task_launcher,
							&start_time);
  }

  g_message("playback stopped");

}

void
tic_callback(GObject *soundcard, gpointer user_data)
{
  AgsFrameClock *frame_clock;

  guint i;
  
  frame_clock = ags_soundcard_get_frame_clock(AGS_SOUNDCARD(soundcard));

  g_message("tic() - frame clock [thread: %d, playing: %d, uptime: %s] - note offset %llu - note 256th offset %llu - length %d - frame offset %llu",
	    ags_atomic_int_get(&(audio_application_context->default_soundcard_thread->is_running)),
	    ags_soundcard_is_playing(AGS_SOUNDCARD(soundcard)),
	    ags_soundcard_get_uptime(AGS_SOUNDCARD(soundcard)),
	    frame_clock->note_offset,
	    frame_clock->note_256th_offset[0],
	    frame_clock->note_256th_offset_length,
	    frame_clock->frame_offset);
  
  for(i = 0; i < frame_clock->note_256th_offset_length; i++){
    g_message("`- note 256th %llu - %llu", frame_clock->note_256th_offset[i], frame_clock->note_256th_frame_offset[i]);
  }
  
  g_message("tic() - frame clock [has 16th pulse: %d - attack: %d - delay counter: %f]", frame_clock->has_16th_pulse, (frame_clock->note_frame_offset % frame_clock->buffer_size), frame_clock->delay_counter);
}

void
activate(GApplication *app,
	 gpointer user_data)
{
  //empty
}

int
main(int argc, char **argv)
{
  GApplication *app;
  
  app = g_application_new("org.nongnu.gsequencer.ags_functional_frame_clock_test",
			  G_APPLICATION_FLAGS_NONE);
  
  g_signal_connect(app, "activate",
		   G_CALLBACK(activate), NULL);
  
  add_thread = g_thread_new("libags_audio.so - functional frame clock test",
			    ags_functional_frame_clock_test_add_thread,
			    NULL);

  g_application_run(G_APPLICATION(app),
		    0, NULL);
  
  g_thread_join(add_thread);
  
  return(-1);
}

