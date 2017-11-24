/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/lib/ags_time.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>
#include <ags/object/ags_seekable.h>

#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_audio_application_context.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_playable.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_play_notation_audio.h>
#include <ags/audio/recall/ags_play_notation_audio_run.h>
#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_channel_run_master.h>
#include <ags/audio/recall/ags_play_channel_run.h>
#include <ags/audio/recall/ags_stream_channel.h>
#include <ags/audio/recall/ags_stream_channel_run.h>
#include <ags/audio/recall/ags_buffer_channel.h>
#include <ags/audio/recall/ags_buffer_channel_run.h>
#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_volume_channel_run.h>
#include <ags/audio/recall/ags_mute_channel.h>
#include <ags/audio/recall/ags_mute_channel_run.h>

#include <ags/audio/task/ags_start_soundcard.h>
#include <ags/audio/task/ags_init_audio.h>
#include <ags/audio/task/ags_append_audio.h>
#include <ags/audio/task/ags_cancel_audio.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

int ags_functional_audio_test_init_suite();
int ags_functional_audio_test_clean_suite();

void ags_functional_audio_test_playback();

#define AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS (2)
#define AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_PADS (78)
#define AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO (4)

#define AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_BUFFER (5)

#define AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_NOTES (4 * (guint) (1.0 / AGS_NOTATION_MINIMUM_NOTE_LENGTH) * 120)
#define AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_STOP_DELAY (120)

#define AGS_FUNCTIONAL_AUDIO_TEST_CONFIG "[generic]\n"	\
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
  "samplerate=48000\n"					\
  "buffer-size=1024\n"					\
  "pcm-channels=2\n"					\
  "dsp-channels=2\n"					\
  "format=16\n"						\
  "\n"							\
  "[recall]\n"						\
  "auto-sense=true\n"					\
  "\n"

AgsAudioApplicationContext *audio_application_context;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_audio_test_init_suite()
{
  AgsConfig *config;

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_FUNCTIONAL_AUDIO_TEST_CONFIG,
			    strlen(AGS_FUNCTIONAL_AUDIO_TEST_CONFIG));
  audio_application_context = ags_audio_application_context_new();
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_audio_test_clean_suite()
{
  g_object_unref(audio_application_context);
  
  return(0);
}

void
ags_functional_audio_test_playback()
{
  AgsAudio *panel, *mixer;
  AgsAudio **audio;
  AgsChannel *channel, *link, *current;

  AgsTaskThread *task_thread;

  AgsThread *audio_loop, *soundcard_thread;
  
  GObject *soundcard;

  GList *start_queue;
  GList *list;

  struct timespec start_time;
  
  guint i, j, k;
  
  GError *error;

  auto void ags_functional_audio_test_playback_add_sink(AgsAudio *audio);
  auto void ags_functional_audio_test_playback_add_mixer(AgsAudio *audio);
  auto void ags_functional_audio_test_playback_add_playback(AgsAudio *audio);
  auto void ags_functional_audio_test_playback_start_audio(AgsAudio *audio);
  auto void ags_functional_audio_test_playback_stop_audio(AgsAudio *audio);

  void ags_functional_audio_test_playback_add_sink(AgsAudio *audio){
    AgsChannel *channel;
    AgsPlayChannel *play_channel;
    AgsPlayChannelRunMaster *play_channel_run;

    GList *list;

    guint n_recall;
    
    channel = audio->input;

    while(channel != NULL){
      /* ags-play */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-play-master\0",
				channel->audio_channel, channel->audio_channel + 1,
				channel->pad, channel->pad + 1,
				(AGS_RECALL_FACTORY_INPUT,
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_ADD),
				0);

      /* set audio channel */
      list = channel->play;
      n_recall = 0;
      
      while((list = ags_recall_template_find_type(list,
						  AGS_TYPE_PLAY_CHANNEL)) != NULL){
	GValue audio_channel_value = {0,};

	CU_ASSERT(AGS_IS_PLAY_CHANNEL(list->data));

	play_channel = AGS_PLAY_CHANNEL(list->data);

	CU_ASSERT(AGS_IS_PORT(play_channel->audio_channel));

	g_value_init(&audio_channel_value, G_TYPE_UINT64);
	g_value_set_uint64(&audio_channel_value,
			   channel->audio_channel);
	ags_port_safe_write(play_channel->audio_channel,
			    &audio_channel_value);
	g_value_unset(&audio_channel_value);
	
	list = list->next;
	n_recall++;
      }

      CU_ASSERT(n_recall == 1);

      channel = channel->next;
    }
  }
  
  void ags_functional_audio_test_playback_add_mixer(AgsAudio *audio){
    AgsChannel *channel;

    GList *list;

    guint n_recall;
    
    channel = audio->input;

    while(channel != NULL){
      /* ags-mute */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-mute\0",
				channel->audio_channel, channel->audio_channel + 1,
				channel->pad, channel->pad + 1,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_RECALL |
				 AGS_RECALL_FACTORY_ADD),
				0);

      list = channel->play;
      n_recall = 0;
      
      while((list = ags_recall_template_find_type(list,
						  AGS_TYPE_MUTE_CHANNEL)) != NULL){
	CU_ASSERT(AGS_IS_MUTE_CHANNEL(list->data));

      	CU_ASSERT(AGS_IS_PORT(AGS_MUTE_CHANNEL(list->data)->muted));

	list = list->next;
	n_recall++;
      }

      CU_ASSERT(n_recall == 1);

      list = channel->recall;
      n_recall = 0;
      
      while((list = ags_recall_template_find_type(list,
						  AGS_TYPE_MUTE_CHANNEL)) != NULL){
	CU_ASSERT(AGS_IS_MUTE_CHANNEL(list->data));

      	CU_ASSERT(AGS_IS_PORT(AGS_MUTE_CHANNEL(list->data)->muted));

	list = list->next;
	n_recall++;
      }

      CU_ASSERT(n_recall == 1);

      /* ags-volume */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-volume\0",
				channel->audio_channel, channel->audio_channel + 1,
				channel->pad, channel->pad + 1,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_RECALL |
				 AGS_RECALL_FACTORY_ADD),
				0);


      list = channel->play;
      n_recall = 0;
      
      while((list = ags_recall_template_find_type(list,
						  AGS_TYPE_VOLUME_CHANNEL)) != NULL){
	CU_ASSERT(AGS_IS_VOLUME_CHANNEL(list->data));

      	CU_ASSERT(AGS_IS_PORT(AGS_VOLUME_CHANNEL(list->data)->volume));

	list = list->next;
	n_recall++;
      }

      CU_ASSERT(n_recall == 1);

      list = channel->recall;
      n_recall = 0;
      
      while((list = ags_recall_template_find_type(list,
						  AGS_TYPE_VOLUME_CHANNEL)) != NULL){
	CU_ASSERT(AGS_IS_VOLUME_CHANNEL(list->data));

      	CU_ASSERT(AGS_IS_PORT(AGS_VOLUME_CHANNEL(list->data)->volume));

	list = list->next;
	n_recall++;
      }

      CU_ASSERT(n_recall == 1);

      channel = channel->next;
    }
  }
  
  void ags_functional_audio_test_playback_add_playback(AgsAudio *audio){
    AgsChannel *channel, *input;

    AgsDelayAudio *play_delay_audio;
    AgsDelayAudioRun *play_delay_audio_run;
    AgsCountBeatsAudio *play_count_beats_audio;
    AgsCountBeatsAudioRun *play_count_beats_audio_run;
    AgsPlayNotationAudio *recall_notation_audio;
    AgsPlayNotationAudioRun *recall_notation_audio_run;
    AgsBufferChannel *buffer_channel;
    AgsBufferChannelRun *buffer_channel_run;

    GList *list;

    guint n_recall;
    
    /* ags-delay */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-delay\0",
			      0, 0,
			      0, 0,
			      (AGS_RECALL_FACTORY_OUTPUT |
			       AGS_RECALL_FACTORY_ADD |
			       AGS_RECALL_FACTORY_PLAY),
			      0);
    list = audio->play;
    n_recall = 0;
      
    while((list = ags_recall_template_find_type(list,
						AGS_TYPE_DELAY_AUDIO)) != NULL){
      CU_ASSERT(AGS_IS_DELAY_AUDIO(list->data));

      play_delay_audio = list->data;
      
      CU_ASSERT(AGS_IS_PORT(AGS_DELAY_AUDIO(list->data)->notation_delay));
      CU_ASSERT(AGS_IS_PORT(AGS_DELAY_AUDIO(list->data)->sequencer_delay));

      CU_ASSERT(AGS_IS_PORT(AGS_DELAY_AUDIO(list->data)->notation_duration));
      CU_ASSERT(AGS_IS_PORT(AGS_DELAY_AUDIO(list->data)->sequencer_duration));

      list = list->next;
      n_recall++;
    }

    CU_ASSERT(n_recall == 1);

    list = ags_recall_find_type(audio->play, AGS_TYPE_DELAY_AUDIO_RUN);

    if(list != NULL){
      play_delay_audio_run = AGS_DELAY_AUDIO_RUN(list->data);
    }    

    /* ags-count-beats */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-count-beats\0",
			      0, 0,
			      0, 0,
			      (AGS_RECALL_FACTORY_OUTPUT |
			       AGS_RECALL_FACTORY_ADD |
			       AGS_RECALL_FACTORY_PLAY),
			      0);

    list = audio->play;
    n_recall = 0;
      
    while((list = ags_recall_template_find_type(list,
						AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
      CU_ASSERT(AGS_IS_COUNT_BEATS_AUDIO(list->data));

      play_count_beats_audio = list->data;
      
      CU_ASSERT(AGS_IS_PORT(AGS_COUNT_BEATS_AUDIO(list->data)->notation_loop));
      CU_ASSERT(AGS_IS_PORT(AGS_COUNT_BEATS_AUDIO(list->data)->notation_loop_start));
      CU_ASSERT(AGS_IS_PORT(AGS_COUNT_BEATS_AUDIO(list->data)->notation_loop_end));

      CU_ASSERT(AGS_IS_PORT(AGS_COUNT_BEATS_AUDIO(list->data)->sequencer_loop));
      CU_ASSERT(AGS_IS_PORT(AGS_COUNT_BEATS_AUDIO(list->data)->sequencer_loop_start));
      CU_ASSERT(AGS_IS_PORT(AGS_COUNT_BEATS_AUDIO(list->data)->sequencer_loop_end));

      list = list->next;
      n_recall++;
    }

    CU_ASSERT(n_recall == 1);
  
    list = ags_recall_find_type(audio->play, AGS_TYPE_COUNT_BEATS_AUDIO_RUN);

    if(list != NULL){
      GValue value = {0,};

      CU_ASSERT(AGS_IS_COUNT_BEATS_AUDIO_RUN(list->data));

      play_count_beats_audio_run = list->data;
      
      /* set dependency */  
      g_object_set(G_OBJECT(play_count_beats_audio_run),
		   "delay-audio-run\0", play_delay_audio_run,
		   NULL);

      ags_seekable_seek(AGS_SEEKABLE(play_count_beats_audio_run),
			0,
			TRUE);

      g_value_init(&value, G_TYPE_BOOLEAN);
      g_value_set_boolean(&value, FALSE);
      ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(play_count_beats_audio_run)->recall_audio)->notation_loop,
			  &value);
      g_value_unset(&value);
    }

    /* ags-play-notation */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-play-notation\0",
			      0, 0,
			      0, 0,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_ADD |
			       AGS_RECALL_FACTORY_RECALL),
			      0);

    list = audio->recall;
    n_recall = 0;
      
    while((list = ags_recall_template_find_type(list,
						AGS_TYPE_PLAY_NOTATION_AUDIO)) != NULL){
      CU_ASSERT(AGS_IS_PLAY_NOTATION_AUDIO(list->data));

      list = list->next;
      n_recall++;
    }

    CU_ASSERT(n_recall == 1);
  

    list = ags_recall_find_type(audio->recall, AGS_TYPE_PLAY_NOTATION_AUDIO_RUN);

    if(list != NULL){
      recall_notation_audio_run = AGS_PLAY_NOTATION_AUDIO_RUN(list->data);
    
      /* set dependency */
      g_object_set(G_OBJECT(recall_notation_audio_run),
		   "delay-audio-run\0", play_delay_audio_run,
		   NULL);

      /* set dependency */
      g_object_set(G_OBJECT(recall_notation_audio_run),
		   "count-beats-audio-run\0", play_count_beats_audio_run,
		   NULL);
    }

    channel = audio->output;

    while(channel != NULL){
      /* ags-stream */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-stream\0",
				channel->audio_channel, channel->audio_channel + 1, 
				channel->pad, channel->pad + 1,
				(AGS_RECALL_FACTORY_OUTPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_RECALL | 
				 AGS_RECALL_FACTORY_ADD),
				0);
      
      input = ags_channel_pad_nth(audio->input,
				  channel->pad);

      while(input != NULL){
	/* ags-buffer */
	ags_recall_factory_create(audio,
				  NULL, NULL,
				  "ags-buffer\0",
				  0, audio->audio_channels, 
				  input->pad, input->pad + 1,
				  (AGS_RECALL_FACTORY_INPUT |
				   AGS_RECALL_FACTORY_RECALL |
				   AGS_RECALL_FACTORY_ADD),
				  0);

	input = input->next_pad;
      }

      channel = channel->next;
    }

    channel = audio->input;

    while(channel != NULL){
      /* ags-stream */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-stream\0",
				channel->audio_channel, channel->audio_channel + 1, 
				channel->pad, channel->pad + 1,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_RECALL | 
				 AGS_RECALL_FACTORY_ADD),
				0);
  
      channel = channel->next;
    }
  }

  void ags_functional_audio_test_playback_start_audio(AgsAudio *audio){
    AgsInitAudio *init_audio;
    AgsAppendAudio *append_audio;
    AgsStartSoundcard *start_soundcard;

    GList *task;

    task = NULL;    
    init_audio = ags_init_audio_new(audio,
				    FALSE, FALSE, TRUE);
    task = g_list_prepend(task,
			  init_audio);
    
    append_audio = ags_append_audio_new(AGS_APPLICATION_CONTEXT(audio_application_context)->main_loop,
					audio,
					FALSE, FALSE, TRUE);
    task = g_list_prepend(task,
			  append_audio);
    
    start_soundcard = ags_start_soundcard_new(audio_application_context);
    task = g_list_prepend(task,
			  start_soundcard);
    
    ags_task_thread_append_tasks(task_thread,
				 task);
  }
  
  void ags_functional_audio_test_playback_stop_audio(AgsAudio *audio){
    AgsCancelAudio *cancel_audio;

    struct timespec sleep_time;
    
    clock_gettime(CLOCK_MONOTONIC, &sleep_time);

    while(sleep_time.tv_sec < start_time.tv_sec + AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_STOP_DELAY){
      usleep(USEC_PER_SEC);
      clock_gettime(CLOCK_MONOTONIC, &sleep_time);

      g_message("ags_functional_audio_test_playback_stop_audio() - usleep %ds [%x]\0",
		start_time.tv_sec + AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_STOP_DELAY - sleep_time.tv_sec,
		audio);
    }
    
    /* create cancel task */
    cancel_audio = ags_cancel_audio_new(audio,
					FALSE, FALSE, TRUE);
    
    /* append AgsCancelAudio */
    ags_task_thread_append_task((AgsTaskThread *) task_thread,
				(AgsTask *) cancel_audio);
  }

  /*
   * Setup audio tree sink, mixer and notation player as source
   */
  soundcard = NULL;
  
  if(audio_application_context->soundcard != NULL){
    soundcard = audio_application_context->soundcard->data;
  }

  /* the output panel */
  panel = ags_audio_new(soundcard);
  panel->flags |= (AGS_AUDIO_SYNC);

  panel->audio_channels = AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS;
  
  ags_audio_set_pads(panel,
		     AGS_TYPE_OUTPUT,
		     1);
  ags_audio_set_pads(panel,
		     AGS_TYPE_INPUT,
		     1);

  list = ags_soundcard_get_audio(AGS_SOUNDCARD(soundcard));
  ags_soundcard_set_audio(AGS_SOUNDCARD(soundcard),
			  g_list_prepend(list,
					 panel));
  
  ags_functional_audio_test_playback_add_sink(panel);

  ags_connectable_connect(AGS_CONNECTABLE(panel));
  
  /* assert recycling NULL */
  channel = panel->output;
  
  for(i = 0; i < AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS; i++){
    CU_ASSERT(channel->first_recycling == NULL);
    CU_ASSERT(channel->last_recycling == NULL);
    
    channel = channel->next;
  }

  channel = panel->input;
  
  for(i = 0; i < AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS; i++){
    CU_ASSERT(channel->first_recycling == NULL);
    CU_ASSERT(channel->last_recycling == NULL);
    
    channel = channel->next;
  }

  /* the mixer */
  mixer = ags_audio_new(soundcard);
  mixer->flags |= (AGS_AUDIO_ASYNC);

  mixer->audio_channels = AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS;
  
  ags_audio_set_pads(mixer,
		     AGS_TYPE_OUTPUT,
		     1);
  ags_audio_set_pads(mixer,
		     AGS_TYPE_INPUT,
		     AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO);

  list = ags_soundcard_get_audio(AGS_SOUNDCARD(soundcard));
  ags_soundcard_set_audio(AGS_SOUNDCARD(soundcard),
			  g_list_prepend(list,
					 mixer));
  
  ags_functional_audio_test_playback_add_mixer(mixer);

  ags_connectable_connect(AGS_CONNECTABLE(mixer));

  /* set link, assert link set and recycling NULL */
  channel = mixer->output;
  link = panel->input;
  
  for(i = 0; i < AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS; i++){
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
  audio = (AgsAudio **) malloc(AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO * sizeof(AgsAudio));

  for(i = 0; i < AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO; i++){
    AgsNotation *notation;
    
    audio[i] = ags_audio_new(soundcard);
    audio[i]->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			AGS_AUDIO_INPUT_HAS_RECYCLING |
			AGS_AUDIO_SYNC |
			AGS_AUDIO_ASYNC |
			AGS_AUDIO_HAS_NOTATION | 
			AGS_AUDIO_NOTATION_DEFAULT);
    
    ags_audio_set_audio_channels(audio[i],
				 AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS);
  
    ags_audio_set_pads(audio[i],
		       AGS_TYPE_OUTPUT,
		       1);
    ags_audio_set_pads(audio[i],
		       AGS_TYPE_INPUT,
		       AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_PADS);

    list = ags_soundcard_get_audio(AGS_SOUNDCARD(soundcard));
    ags_soundcard_set_audio(AGS_SOUNDCARD(soundcard),
			    g_list_prepend(list,
					   audio[i]));
    
    /* populate notation and set link, assert link set and recycling not NULL */
    channel = audio[i]->output;
    link = ags_channel_pad_nth(mixer->input,
			       i);
  
    for(j = 0; j < AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS; j++){
      AgsAudioSignal *destination;

      destination = ags_audio_signal_new_with_length(soundcard,
						     channel->first_recycling,
						     NULL,
						     AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_BUFFER);
      destination->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
      ags_recycling_add_audio_signal(channel->first_recycling,
				     destination);
      
      /* populate notation */
      notation = g_list_nth(audio[i]->notation,
			    j)->data;

      for(k = 0; k < AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_NOTES; k++){
	AgsNote *note;

	note = ags_note_new();
	note->x[0] = k + (rand() % 4);
	note->x[1] = note->x[0] + (rand() % 3) + 1;
	note->y = rand() % AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_PADS;

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
    
    for(j = 0; j < AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_PADS; j++){
      AgsAudioSignal *source;

      source = ags_audio_signal_new_with_length(soundcard,
						channel->first_recycling,
						NULL,
						AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_BUFFER);
      source->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
      ags_recycling_add_audio_signal(channel->first_recycling,
				     source);

      channel = channel->next;
    }
    
    /* find recycling within mixer */
    channel = audio[i]->output;
    current = mixer->output;

    for(j = 0; j < AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS; j++){
      CU_ASSERT(ags_recycling_position(current->first_recycling, current->last_recycling->next,
				       channel->first_recycling) != -1);

      channel = channel->next;
      current = current->next;
    }

    /* find recycling within panel */
    channel = audio[i]->output;
    current = panel->input;

    for(j = 0; j < AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS; j++){
      CU_ASSERT(ags_recycling_position(current->first_recycling, current->last_recycling->next,
				       channel->first_recycling) != -1);

      channel = channel->next;
      current = current->next;
    }

    ags_functional_audio_test_playback_add_playback(audio[i]);

    ags_connectable_connect(AGS_CONNECTABLE(audio[i]));
  }
  
  /*
   * Start threads and enable playback
   */
  audio_loop = AGS_APPLICATION_CONTEXT(audio_application_context)->main_loop;
  task_thread = ags_thread_find_type(audio_loop,
				     AGS_TYPE_TASK_THREAD);
  soundcard_thread = ags_thread_find_type(audio_loop,
					  AGS_TYPE_SOUNDCARD_THREAD);

  /* start engine */
  pthread_mutex_lock(audio_loop->start_mutex);
    
  start_queue = NULL;
  start_queue = g_list_prepend(start_queue,
			       task_thread);
  g_atomic_pointer_set(&(audio_loop->start_queue),
		       start_queue);
  
  pthread_mutex_unlock(audio_loop->start_mutex);

  
  ags_thread_start(audio_loop);
  ags_thread_pool_start(audio_application_context->thread_pool);

  /* wait for audio loop */
  pthread_mutex_lock(audio_loop->start_mutex);

  if(g_atomic_int_get(&(audio_loop->start_wait)) == TRUE){	
    g_atomic_int_set(&(audio_loop->start_done),
		     FALSE);
      
    while(g_atomic_int_get(&(audio_loop->start_wait)) == TRUE &&
	  g_atomic_int_get(&(audio_loop->start_done)) == FALSE){
      pthread_cond_wait(audio_loop->start_cond,
			audio_loop->start_mutex);
    }
  }
    
  pthread_mutex_unlock(audio_loop->start_mutex);

  /* start playback */
  g_message("start playback\0");
  
  for(i = 0; i < AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO; i++){
    ags_functional_audio_test_playback_start_audio(audio[i]);
  }

  clock_gettime(CLOCK_MONOTONIC, &start_time);

  /* stop playback */
  for(i = 0; i < AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO; i++){
    ags_functional_audio_test_playback_stop_audio(audio[i]);
  }

  g_message("playback stopped\0");
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C\0");
  putenv("LANG=C\0");

  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsAudioTest\0", ags_functional_audio_test_init_suite, ags_functional_audio_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsAudio doing playback\0", ags_functional_audio_test_playback) == NULL)){
      CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

