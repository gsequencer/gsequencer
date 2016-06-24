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

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>
#include <ags/object/ags_seekable.h>

#include <ags/audio/ags_audio_application_context.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_playable.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_channel_run_master.h>
#include <ags/audio/recall/ags_play_channel_run.h>
#include <ags/audio/recall/ags_stream_channel.h>
#include <ags/audio/recall/ags_stream_channel_run.h>
#include <ags/audio/recall/ags_buffer_channel.h>
#include <ags/audio/recall/ags_buffer_channel_run.h>
#include <ags/audio/recall/ags_play_notation_audio.h>
#include <ags/audio/recall/ags_play_notation_audio_run.h>

int ags_functional_audio_test_init_suite();
int ags_functional_audio_test_clean_suite();

void ags_functional_audio_test_playback();

#define AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS (2)
#define AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_PADS (78)
#define AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO (4)

AgsAudioApplicationContext *audio_application_context;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_audio_test_init_suite()
{
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
  AgsAudio *audio;
  AgsChannel *channel, *link, *current;
  
  GObject *soundcard;

  guint i, j;
  
  GError *error;

  auto void ags_functional_audio_test_playback_add_sink(AgsAudio *audio);
  auto void ags_functional_audio_test_playback_add_mixer(AgsAudio *audio);
  auto void ags_functional_audio_test_playback_add_playback(AgsAudio *audio);

  void ags_functional_audio_test_playback_add_sink(AgsAudio *audio){
    AgsChannel *channel;
    AgsPlayChannel *play_channel;
    AgsPlayChannelRunMaster *play_channel_run;

    GList *list;

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

      while((list = ags_recall_template_find_type(list,
						  AGS_TYPE_PLAY_CHANNEL)) != NULL){
	GValue audio_channel_value = {0,};

	play_channel = AGS_PLAY_CHANNEL(list->data);

	g_value_init(&audio_channel_value, G_TYPE_UINT64);
	g_value_set_uint64(&audio_channel_value,
			   channel->audio_channel);
	ags_port_safe_write(play_channel->audio_channel,
			    &audio_channel_value);
	g_value_unset(&audio_channel_value);
	
	list = list->next;
      }

      channel = channel->next;
    }
  }
  
  void ags_functional_audio_test_playback_add_mixer(AgsAudio *audio){
    AgsChannel *channel;

    GList *list;

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


      channel = channel->next;
    }
  }
  
  void ags_functional_audio_test_playback_add_playback(AgsAudio *audio){
    AgsChannel *channel;

    AgsDelayAudio *play_delay_audio;
    AgsDelayAudioRun *play_delay_audio_run;
    AgsCountBeatsAudio *play_count_beats_audio;
    AgsCountBeatsAudioRun *play_count_beats_audio_run;
    AgsPlayNotationAudio *recall_notation_audio;
    AgsPlayNotationAudioRun *recall_notation_audio_run;

    GList *list;

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

    list = ags_recall_find_type(audio->play, AGS_TYPE_DELAY_AUDIO_RUN);

    if(list != NULL){
      play_delay_audio_run = AGS_DELAY_AUDIO_RUN(list->data);
      //    AGS_RECALL(play_delay_audio_run)->flags |= AGS_RECALL_PERSISTENT;
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
  
    list = ags_recall_find_type(audio->play, AGS_TYPE_COUNT_BEATS_AUDIO_RUN);

    if(list != NULL){
      GValue value = {0,};

      play_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(list->data);

      /* set dependency */  
      g_object_set(G_OBJECT(play_count_beats_audio_run),
		   "delay-audio-run\0", play_delay_audio_run,
		   NULL);
      ags_seekable_seek(AGS_SEEKABLE(play_count_beats_audio_run),
			0.0,
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

  }
  
  soundcard = audio_application_context->soundcard->data;

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

  ags_functional_audio_test_playback_add_sink(panel);
  
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

  ags_functional_audio_test_playback_add_playback(mixer);
  
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
  for(i = 0; i < AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO; i++){
    audio = ags_audio_new(soundcard);
    audio->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		     AGS_AUDIO_INPUT_HAS_RECYCLING |
		     AGS_AUDIO_SYNC |
		     AGS_AUDIO_ASYNC |
		     AGS_AUDIO_HAS_NOTATION | 
		     AGS_AUDIO_NOTATION_DEFAULT);
    
    audio->audio_channels = AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS;
  
    ags_audio_set_pads(audio,
		       AGS_TYPE_OUTPUT,
		       1);
    ags_audio_set_pads(audio,
		       AGS_TYPE_INPUT,
		       AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_PADS);

    /* set link, assert link set and recycling not NULL */
    channel = audio->output;
    link = ags_channel_pad_nth(mixer->input,
			       i);
  
    for(j = 0; j < AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS; j++){
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

    /* find recycling within mixer */
    channel = audio->output;
    current = mixer->output;

    for(j = 0; j < AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS; j++){
      CU_ASSERT(ags_recycling_position(current->first_recycling, current->last_recycling->next,
				       channel->first_recycling) != -1);

      channel = channel->next;
      current = current->next;
    }

    /* find recycling within panel */
    channel = audio->output;
    current = panel->input;

    for(j = 0; j < AGS_FUNCTIONAL_AUDIO_TEST_PLAYBACK_N_AUDIO_CHANNELS; j++){
      CU_ASSERT(ags_recycling_position(current->first_recycling, current->last_recycling->next,
				       channel->first_recycling) != -1);

      channel = channel->next;
      current = current->next;
    }

    ags_functional_audio_test_playback_add_playback(audio);
  }
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

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

