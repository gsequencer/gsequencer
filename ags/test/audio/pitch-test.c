/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <math.h>
#include <stdio.h>

void test_init();

void test_generate_sine_buffer();

void test_compute_alias_sine_buffer();
void test_compute_alias_new_sine_buffer();
void test_compute_new_sine_buffer();

void test_play_sine_buffer();
void test_play_new_sine_buffer();

#define TEST_AUDIO_CHANNELS (2)
#define TEST_BUFFER_SIZE (2048)
#define TEST_SAMPLERATE (44100.0)
#define TEST_FORMAT (16)

#define TEST_FRAME_COUNT (8 * 44100.0)
#define TEST_COPY_MODE (AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16)

#define TEST_SINE_FREQ (440.0)
#define TEST_NEW_SINE_FREQ (220.0)

#define TEST_DELAY (8 * G_USEC_PER_SEC)

#define ALIAS (16.0)
#define BAND (22000.0)

#define TEST_CONFIG "[generic]\n"	\
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
  "thread-pool-max-unused-threads=4\n"			\
  "max-precision=250\n"					\
  "\n"							\
  "[soundcard-0]\n"					\
  "backend=alsa\n"					\
  "capability=playback\n"				\
  "device=hw:CARD=CODEC,DEV=0\n"			\
  "samplerate=44100\n"					\
  "buffer-size=2048\n"					\
  "pcm-channels=2\n"					\
  "dsp-channels=2\n"					\
  "format=16\n"						\
  "use-cache=false\n"					\
  "cache-buffer-size=8192\n"				\
  "\n"							\
  "[recall]\n"						\
  "auto-sense=true\n"					\
  "\n"


AgsAudioApplicationContext *audio_application_context;

AgsAudio *output_panel;
AgsAudio *wave_player;

GObject *output_soundcard;

gpointer test_sine_buffer;

gpointer test_alias_sine_buffer;
gpointer test_alias_new_sine_buffer;

gpointer test_new_sine_buffer;

gdouble sine_freq = TEST_SINE_FREQ;
gdouble new_sine_freq = TEST_NEW_SINE_FREQ;

void
test_init()
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
			      TEST_CONFIG,
			      strlen(TEST_CONFIG));
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
			       TEST_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(output_panel,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(output_panel,
		     AGS_TYPE_INPUT,
		     1, 0);

  /* ags-fx-playback */
  for(i = 0; i < TEST_AUDIO_CHANNELS; i++){
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
			       TEST_AUDIO_CHANNELS, 0);
  
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
  
  for(i = 0; i < TEST_AUDIO_CHANNELS; i++){
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
  for(i = 0; i < TEST_AUDIO_CHANNELS; i++){
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
  
  /* link */
  channel = output_panel->input;
  link = wave_player->output;
  
  for(i = 0; i < TEST_AUDIO_CHANNELS; i++){
    error = NULL;
    ags_channel_set_link(channel,
			 link,
			 &error);

    channel = channel->next;
    link = link->next;
  }

  /* buffers */
  test_sine_buffer = g_malloc(TEST_FRAME_COUNT * sizeof(gint16));
    
  test_alias_sine_buffer = g_malloc(ALIAS * TEST_FRAME_COUNT * sizeof(gint16));
  test_alias_new_sine_buffer = g_malloc(ALIAS * TEST_FRAME_COUNT * sizeof(gint16));

  test_new_sine_buffer = g_malloc(TEST_FRAME_COUNT * sizeof(gint16));
}

void
test_generate_sine_buffer()
{
  gint i;

  for(i = 0; i < TEST_FRAME_COUNT; i++){
    ((gint16 *) test_sine_buffer)[i] = (gint16) (32000.0 * sin((double) i * 2.0 * M_PI * sine_freq / TEST_SAMPLERATE));
  }
}

void
test_compute_alias_sine_buffer()
{
  gint i;
  gint j;
  
  g_message("computing alias buffer");
  
  for(i = 0; i < 16 * TEST_FRAME_COUNT; i++){
    gdouble t;

    t = (double) (i % 16) / 16.0;

    if((i / 16) + 1 < TEST_FRAME_COUNT){
      ((gint16 *) test_alias_sine_buffer)[i] = ((1.0 - t) * ((gint16 *) test_sine_buffer)[i / 16]) + (t * ((gint16 *) test_sine_buffer)[(i / 16) + 1]);
    }
  }
}

void
test_compute_alias_new_sine_buffer()
{
  gdouble sine_freq_period;
  gdouble new_sine_freq_period;
  gdouble volume;
  gint reset_i;
  gint i;

  sine_freq_period = (16 * TEST_SAMPLERATE) / sine_freq;

  new_sine_freq_period = (16 * TEST_SAMPLERATE) / new_sine_freq;

  reset_i = -1;
  
  for(i = 0; i < 16 * TEST_FRAME_COUNT; i++){
    gdouble t;

    if(sine_freq_period < new_sine_freq_period){
      t = (i % (guint) sine_freq_period) / new_sine_freq_period;

      if((guint) floor(i * (sine_freq_period / new_sine_freq_period)) < 16 * TEST_FRAME_COUNT){
	((gint16 *) test_alias_new_sine_buffer)[i] = ((1.0 - t) * ((gint16 *) test_alias_sine_buffer)[i]) + (t * ((gint16 *) test_alias_sine_buffer)[(guint) floor(i * (sine_freq_period / new_sine_freq_period))]);
      }
    }else{
      t = 1.0 / sine_freq_period * new_sine_freq_period;

      if(i * sine_freq_period / new_sine_freq_period < 16 * TEST_FRAME_COUNT){
	((gint16 *) test_alias_new_sine_buffer)[i] = ((gint16 *) test_alias_sine_buffer)[(guint) floor(i * sine_freq_period / new_sine_freq_period)];
      }else{
	if(reset_i == -1){
	  reset_i = i;
	}

	if(reset_i != -1 && i - reset_i >= reset_i){
	  reset_i = i;
	}
	
	((gint16 *) test_alias_new_sine_buffer)[i] = ((gint16 *) test_alias_sine_buffer)[(guint) floor((i - reset_i) * sine_freq_period / new_sine_freq_period)];
      }
    }
  }
}

void
test_compute_new_sine_buffer()
{
  gint i;
  
  for(i = 0; i < TEST_FRAME_COUNT - 1; i++){
    ((gint16 *) test_new_sine_buffer)[i] = ((gint16 *) test_alias_new_sine_buffer)[16 * i + 1];
  }
}

void
test_play_sine_buffer()
{
  AgsStartAudio *start_audio;
  AgsStartSoundcard *start_soundcard;
  AgsCancelAudio *cancel_audio;

  AgsAudioBufferUtil audio_buffer_util;
  
  AgsTaskLauncher *task_launcher;
  
  GList *start_wave, *wave;
  GList *task;

  gint i, j;
  
  start_wave = NULL;
  
  for(i = 0; i < TEST_AUDIO_CHANNELS; i++){
    AgsWave *current_wave;
    
    current_wave = ags_wave_new(wave_player,
				i);

    g_object_set(current_wave,
		 "samplerate", (guint) TEST_SAMPLERATE,
		 "buffer-size", (guint) TEST_BUFFER_SIZE,
		 "format", TEST_FORMAT,
		 NULL);

    start_wave = ags_wave_add(start_wave,
			      current_wave);
    
    for(j = 0; j < floor(TEST_FRAME_COUNT / TEST_BUFFER_SIZE); j++){
      AgsBuffer *buffer;
      
      buffer = ags_buffer_new();
      g_object_set(buffer,
		   "samplerate", (guint) TEST_SAMPLERATE,
		   "buffer-size", (guint) TEST_BUFFER_SIZE,
		   "format", TEST_FORMAT,
		   "x", (guint64) (j * TEST_BUFFER_SIZE),
		   NULL);
      ags_wave_add_buffer(current_wave,
			  buffer,
			  FALSE);

      ags_audio_buffer_util_copy_buffer_to_buffer(&audio_buffer_util,
						  buffer->data, 1, 0,
						  test_sine_buffer, 1, j * TEST_BUFFER_SIZE,
						  TEST_BUFFER_SIZE, TEST_COPY_MODE);

      g_message("xcross %d", ags_synth_util_get_xcross_count_s16(buffer->data, TEST_BUFFER_SIZE));
    }
  }

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(audio_application_context));

  g_message("playing sine buffer at %f Hz", sine_freq);
    
  wave_player->wave = start_wave;

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
  usleep(TEST_DELAY);
    
  /* create cancel task */
  cancel_audio = ags_cancel_audio_new(wave_player,
				      AGS_SOUND_SCOPE_NOTATION);
    
  /* append AgsCancelAudio */
  ags_task_launcher_add_task(task_launcher,
			     (AgsTask *) cancel_audio);
}

void
test_play_new_sine_buffer()
{
  AgsStartAudio *start_audio;
  AgsStartSoundcard *start_soundcard;
  AgsCancelAudio *cancel_audio;

  AgsTaskLauncher *task_launcher;
  
  GList *start_wave, *wave;
  GList *task;

  gint i, j;
  
  start_wave = NULL;
  
  for(i = 0; i < TEST_AUDIO_CHANNELS; i++){
    AgsWave *current_wave;
    
    current_wave = ags_wave_new(wave_player,
				i);

    g_object_set(current_wave,
		 "samplerate", (guint) TEST_SAMPLERATE,
		 "buffer-size", (guint) TEST_BUFFER_SIZE,
		 "format", TEST_FORMAT,
		 NULL);

    start_wave = ags_wave_add(start_wave,
			      current_wave);
    
    for(j = 0; j < floor(TEST_FRAME_COUNT / TEST_BUFFER_SIZE); j++){
      AgsBuffer *buffer;
      
      buffer = ags_buffer_new();
      g_object_set(buffer,
		   "samplerate", (guint) TEST_SAMPLERATE,
		   "buffer-size", (guint) TEST_BUFFER_SIZE,
		   "format", TEST_FORMAT,
		   "x", (guint64) (j * TEST_BUFFER_SIZE),
		   NULL);
      ags_wave_add_buffer(current_wave,
			  buffer,
			  FALSE);

      ags_audio_buffer_util_copy_buffer_to_buffer(&audio_buffer_util,
						  buffer->data, 1, 0,
						  test_new_sine_buffer, 1, j * TEST_BUFFER_SIZE,
						  TEST_BUFFER_SIZE, TEST_COPY_MODE);

      g_message("xcross %d", ags_synth_util_get_xcross_count_s16(buffer->data, TEST_BUFFER_SIZE));
    }
  }

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(audio_application_context));

  g_message("playing new sine buffer at %f Hz", new_sine_freq);
    
  wave_player->wave = start_wave;

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
  usleep(TEST_DELAY);
    
  /* create cancel task */
  cancel_audio = ags_cancel_audio_new(wave_player,
				      AGS_SOUND_SCOPE_NOTATION);
    
  /* append AgsCancelAudio */
  ags_task_launcher_add_task(task_launcher,
			     (AgsTask *) cancel_audio);
}

void*
test_thread()
{
  test_play_sine_buffer();
  test_play_new_sine_buffer();
}

gboolean
timeout(gpointer user_data)
{
  g_thread_new("test pitch", test_thread, NULL);

  
  return(FALSE);
}


int
main(int argc, char **argv)
{
  test_init();
  
  test_generate_sine_buffer();
  
  test_compute_alias_sine_buffer();
  test_compute_alias_new_sine_buffer();

  test_compute_new_sine_buffer();

  g_timeout_add(10000, timeout, NULL);
  
  g_main_loop_run(g_main_loop_new(g_main_context_default(),
				  FALSE));
 
  
  return(0);
}

