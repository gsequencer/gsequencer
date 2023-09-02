/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

int ags_audio_test_init_suite();
int ags_audio_test_clean_suite();

void ags_audio_test_set_soundcard();
void ags_audio_test_set_flags();
void ags_audio_test_unset_flags();
void ags_audio_test_dispose();
void ags_audio_test_finalize();
void ags_audio_test_check_connection();
void ags_audio_test_set_audio_channels();
void ags_audio_test_set_pads();
void ags_audio_test_set_samplerate();
void ags_audio_test_set_buffer_size();
void ags_audio_test_set_format();
void ags_audio_test_set_sequence_length();
void ags_audio_test_add_audio_connection();
void ags_audio_test_remove_audio_connection();
void ags_audio_test_add_preset();
void ags_audio_test_remove_preset();
void ags_audio_test_add_notation();
void ags_audio_test_remove_notation();
void ags_audio_test_add_automation();
void ags_audio_test_remove_automation();
void ags_audio_test_add_recall_id();
void ags_audio_test_remove_recall_id();
void ags_audio_test_add_recycling_context();
void ags_audio_test_remove_recycling_context();
void ags_audio_test_add_recall_container();
void ags_audio_test_remove_recall_container();
void ags_audio_test_add_recall();
void ags_audio_test_remove_recall();
void ags_audio_test_init_run();
void ags_audio_test_duplicate_recall();
void ags_audio_test_init_recall();
void ags_audio_test_resolve_recall();
void ags_audio_test_is_playing();
void ags_audio_test_play();
void ags_audio_test_tact();
void ags_audio_test_done();
void ags_audio_test_cancel();
void ags_audio_test_remove();
void ags_audio_test_find_port();
void ags_audio_test_open_files();
void ags_audio_test_recursive_set_property();
void ags_audio_test_recursive_play_init();
void ags_audio_test_link_channel();
void ags_audio_test_finalize_linked_channel();
void ags_audio_test_init_recall();
void ags_audio_test_resolve_recall();

void ags_audio_test_finalize_stub(GObject *gobject);
void ags_audio_test_set_link_callback(AgsChannel *channel, AgsChannel *link,
				      GError **error,
				      AgsAudio *audio);
void ags_audio_test_init_recall_callback(AgsRecall *recall,
					 gpointer data);
void ags_audio_test_resolve_recall_callback(AgsRecall *recall,
					    gpointer data);

#define AGS_AUDIO_TEST_DISPOSE_AUDIO_CHANNELS (2)
#define AGS_AUDIO_TEST_DISPOSE_INPUT_PADS (12)
#define AGS_AUDIO_TEST_DISPOSE_OUTPUT_PADS (5)

#define AGS_AUDIO_TEST_FINALIZE_AUDIO_CHANNELS (2)
#define AGS_AUDIO_TEST_FINALIZE_INPUT_PADS (12)
#define AGS_AUDIO_TEST_FINALIZE_OUTPUT_PADS (5)

#define AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS (2)
#define AGS_AUDIO_TEST_SET_PADS_INPUT_PADS (12)
#define AGS_AUDIO_TEST_SET_PADS_OUTPUT_PADS (5)
#define AGS_AUDIO_TEST_SET_PADS_GROW_INPUT_PADS (18)
#define AGS_AUDIO_TEST_SET_PADS_GROW_OUTPUT_PADS (8)
#define AGS_AUDIO_TEST_SET_PADS_SHRINK_INPUT_PADS (3)
#define AGS_AUDIO_TEST_SET_PADS_SHRINK_OUTPUT_PADS (1)

#define AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS (3)
#define AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_INPUT_PADS (1)
#define AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_OUTPUT_PADS (7)
#define AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_GROW_AUDIO_CHANNELS (5)
#define AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_SHRINK_AUDIO_CHANNELS (1)

#define AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_AUDIO_CHANNELS (2)
#define AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_INPUT_PADS (8)
#define AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_OUTPUT_PADS (1)
#define AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_0_AUDIO_CHANNELS (2)
#define AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_0_INPUT_PADS (8)
#define AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_0_OUTPUT_PADS (1)
#define AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_1_AUDIO_CHANNELS (1)
#define AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_1_INPUT_PADS (128)
#define AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_1_OUTPUT_PADS (1)
#define AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_2_AUDIO_CHANNELS (2)
#define AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_2_INPUT_PADS (1)
#define AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_2_OUTPUT_PADS (1)

#define AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_AUDIO_CHANNELS (2)
#define AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_INPUT_PADS (8)
#define AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_OUTPUT_PADS (1)
#define AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_0_AUDIO_CHANNELS (2)
#define AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_0_INPUT_PADS (8)
#define AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_0_OUTPUT_PADS (1)
#define AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_1_AUDIO_CHANNELS (2)
#define AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_1_INPUT_PADS (128)
#define AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_1_OUTPUT_PADS (1)
#define AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_2_AUDIO_CHANNELS (2)
#define AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_2_INPUT_PADS (1)
#define AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_2_OUTPUT_PADS (1)

AgsAlsaDevout *devout;
gboolean audio_test_finalized;

struct{
  AgsAudio *master;
  guint n_link_master;

  AgsAudio *slave_0;
  guint n_link_slave_0;

  AgsAudio *slave_1;
  guint n_link_slave_1;
  
  AgsAudio *slave_2;
  guint n_link_slave_2;
}test_link_channel;

struct{
  AgsAudio *master;
  guint n_link_master;

  AgsAudio *slave_0;
  guint n_link_slave_0;

  AgsAudio *slave_1;
  guint n_link_slave_1;
  
  AgsAudio *slave_2;
  guint n_link_slave_2;
}test_finalize_linked_channel;

guint test_init_recall_callback_hits_count = 0;
guint test_resolve_recall_callback_hits_count = 0;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_audio_test_init_suite()
{
  devout = ags_alsa_devout_new();
  g_object_ref(devout);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_audio_test_clean_suite()
{
  g_object_unref(devout);

  return(0);
}

void
ags_audio_test_set_soundcard()
{
  //TODO:JK: implement me
}

void
ags_audio_test_set_flags()
{
  //TODO:JK: implement me
}

void
ags_audio_test_unset_flags()
{
  //TODO:JK: implement me
}

void
ags_audio_test_dispose()
{
  AgsAudio *audio;

  /* instantiate audio */
  audio = ags_audio_new(devout);

  /* set audio channels */
  audio->audio_channels = AGS_AUDIO_TEST_DISPOSE_AUDIO_CHANNELS;

  /* set input pads */
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_DISPOSE_INPUT_PADS, 0);

  /* set output pads */
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_DISPOSE_OUTPUT_PADS, 0);

  /* run dispose and assert */
  g_object_run_dispose(audio);

  CU_ASSERT(audio->output_soundcard == NULL);
  CU_ASSERT(audio->input_sequencer == NULL);

  CU_ASSERT(audio->output_midi_file == NULL);

  CU_ASSERT(audio->output == NULL);
  CU_ASSERT(audio->input == NULL);

  CU_ASSERT(audio->playback_domain == NULL);

  CU_ASSERT(audio->notation == NULL);
  CU_ASSERT(audio->automation == NULL);

  CU_ASSERT(audio->recall_id == NULL);
  CU_ASSERT(audio->recycling_context == NULL);

  CU_ASSERT(audio->recall_container == NULL);
  CU_ASSERT(audio->recall == NULL);
  CU_ASSERT(audio->play == NULL);
}

void
ags_audio_test_finalize()
{
  AgsAudio *audio;

  /* instantiate audio */
  audio = ags_audio_new(devout);

  /* set audio channels */
  audio->audio_channels = AGS_AUDIO_TEST_FINALIZE_AUDIO_CHANNELS;

  /* set input pads */
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_FINALIZE_INPUT_PADS, 0);

  /* set output pads */
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_FINALIZE_OUTPUT_PADS, 0);

  /* run dispose */
  g_object_run_dispose(audio);

  /* stub */
  audio_test_finalized = FALSE;
  G_OBJECT_GET_CLASS(audio)->finalize = ags_audio_test_finalize_stub;
  
  /* unref and assert */
  g_object_unref(audio);
  
  CU_ASSERT(audio_test_finalized == TRUE);
}

void
ags_audio_test_check_connection()
{
  //TODO:JK: implement me
}

void
ags_audio_test_set_audio_channels()
{
  AgsAudio *audio;
  AgsChannel *channel, *current;

  guint i, j;
  
  /* instantiate */
  audio = ags_audio_new(devout);

  CU_ASSERT(audio != NULL);

  /* set pads */
  audio->output_pads = AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_OUTPUT_PADS;
  audio->input_pads = AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_INPUT_PADS;

  /* set audio channels */
  ags_audio_set_audio_channels(audio,
			       AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS, 0);

  CU_ASSERT(audio->audio_channels == AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS);

  /* verify pad count */
  channel = audio->input;
  
  for(i = 0; channel != ags_channel_nth(audio->input,
					AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS); i++){
    current = channel;
    
    for(j = 0; current != NULL; j++){
      current = current->next_pad;
    }

    CU_ASSERT(j == AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_INPUT_PADS);
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS);

  /* verify line count */
  channel = audio->input;
  
  for(i = 0; channel != NULL; i++){
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_INPUT_PADS * AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS);

  /* verify pad count */
  channel = audio->output;
  
  for(i = 0; channel != ags_channel_nth(audio->output,
					AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS); i++){
    current = channel;
    
    for(j = 0; current != NULL; j++){
      current = current->next_pad;
    }
    
    CU_ASSERT(j == AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_OUTPUT_PADS);
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS);

  /* verify line count */
  channel = audio->output;
  
  for(i = 0; channel != NULL; i++){
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_OUTPUT_PADS * AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS);
}

void
ags_audio_test_set_pads()
{
  AgsAudio *audio;
  AgsChannel *channel, *current;

  GList *start_list;

  guint i, j;
  gboolean success_unique;
  
  /* instantiate */
  audio = ags_audio_new(devout);

  CU_ASSERT(audio != NULL);

  /* set audio channels */
  audio->audio_channels = AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS;

  /* set input pads */
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_SET_PADS_INPUT_PADS, 0);

  CU_ASSERT(audio->input_pads == AGS_AUDIO_TEST_SET_PADS_INPUT_PADS);

  /* verify pad count */
  channel = audio->input;
  
  for(i = 0; channel != audio->input->next_pad; i++){
    current = channel;
    
    for(j = 0; current != NULL; j++){
      current = current->next_pad;
    }
    
    CU_ASSERT(j == AGS_AUDIO_TEST_SET_PADS_INPUT_PADS);
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);

  /* verify reverse pad count */
  channel = ags_channel_last(audio->input);
  
  for(i = 0; channel != ags_channel_last(audio->input)->prev_pad; i++){
    current = channel;
    
    for(j = 0; current != NULL; j++){
      current = current->prev_pad;
    }

    g_message("--- %d", j);
    CU_ASSERT(j == AGS_AUDIO_TEST_SET_PADS_INPUT_PADS);
    channel = channel->prev;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);
  
  /* verify line count */
  channel = audio->input;
  
  for(i = 0; channel != NULL; i++){
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_INPUT_PADS * AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);

  /* verify reverse line count */
  channel = ags_channel_last(audio->input);
  
  for(i = 0; channel != NULL; i++){
    channel = channel->prev;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_INPUT_PADS * AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);

  /* set output pads */
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_SET_PADS_OUTPUT_PADS, 0);

  CU_ASSERT(audio->output_pads == AGS_AUDIO_TEST_SET_PADS_OUTPUT_PADS);

  /* verify pad count */
  channel = audio->output;
  
  for(i = 0; channel != audio->output->next_pad; i++){
    current = channel;
    
    for(j = 0; current != NULL; j++){
      current = current->next_pad;
    }
    
    CU_ASSERT(j == AGS_AUDIO_TEST_SET_PADS_OUTPUT_PADS);
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);

  /* verify line count */
  channel = audio->output;
  
  for(i = 0; channel != NULL; i++){
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_OUTPUT_PADS * AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);

  /*
   * grow pads
   */
  /* set input pads */
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_SET_PADS_GROW_INPUT_PADS, 0);

  CU_ASSERT(audio->input_pads == AGS_AUDIO_TEST_SET_PADS_GROW_INPUT_PADS);

  /* verify pad count */
  channel = audio->input;
  
  for(i = 0; channel != audio->input->next_pad; i++){
    current = channel;
    
    for(j = 0; current != NULL; j++){
      current = current->next_pad;
    }
    
    CU_ASSERT(j == AGS_AUDIO_TEST_SET_PADS_GROW_INPUT_PADS);
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);

  /* verify line count */
  channel = audio->input;
  
  for(i = 0; channel != NULL; i++){
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_GROW_INPUT_PADS * AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);

  /* grow again */
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     2 * AGS_AUDIO_TEST_SET_PADS_GROW_INPUT_PADS, 0);

  CU_ASSERT(audio->input_pads == 2 * AGS_AUDIO_TEST_SET_PADS_GROW_INPUT_PADS);
  
  channel = audio->input;

  start_list = NULL;
  success_unique = TRUE;
  
  for(i = 0; channel != NULL; i++){
    if(g_list_find(start_list, channel) == NULL){
      start_list = g_list_prepend(start_list,
				  channel);
    }else{
      success_unique = FALSE;
    }
    
    channel = channel->next;
  }

  CU_ASSERT(success_unique == TRUE);
  CU_ASSERT(i == 2 * AGS_AUDIO_TEST_SET_PADS_GROW_INPUT_PADS * AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);

  channel = audio->input;

  start_list = NULL;
  success_unique = TRUE;
  
  for(i = 0; channel != NULL; i++){
    if(g_list_find(start_list, channel) == NULL){
      start_list = g_list_prepend(start_list,
				  channel);
    }else{
      success_unique = FALSE;
    }

    channel = channel->next_pad;
  }

  CU_ASSERT(success_unique == TRUE);
  CU_ASSERT(i == 2 * AGS_AUDIO_TEST_SET_PADS_GROW_INPUT_PADS);
  
  /* set output pads */
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_SET_PADS_GROW_OUTPUT_PADS, 0);

  CU_ASSERT(audio->output_pads == AGS_AUDIO_TEST_SET_PADS_GROW_OUTPUT_PADS);

  /* verify pad count */
  channel = audio->output;
  
  for(i = 0; channel != audio->output->next_pad; i++){
    current = channel;
    
    for(j = 0; current != NULL; j++){
      current = current->next_pad;
    }
    
    CU_ASSERT(j == AGS_AUDIO_TEST_SET_PADS_GROW_OUTPUT_PADS);
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);

  /* verify line count */
  channel = audio->output;
  
  for(i = 0; channel != NULL; i++){
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_GROW_OUTPUT_PADS * AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);
  
  /*
   * shrink pads
   */
  /* set input pads */
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_SET_PADS_SHRINK_INPUT_PADS, 0);

  CU_ASSERT(audio->input_pads == AGS_AUDIO_TEST_SET_PADS_SHRINK_INPUT_PADS);

  /* verify pad count */
  channel = audio->input;
  
  for(i = 0; channel != audio->input->next_pad; i++){
    current = channel;
    
    for(j = 0; current != NULL; j++){
      current = current->next_pad;
    }
    
    CU_ASSERT(j == AGS_AUDIO_TEST_SET_PADS_SHRINK_INPUT_PADS);
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);

  /* verify line count */
  channel = audio->input;
  
  for(i = 0; channel != NULL; i++){
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_SHRINK_INPUT_PADS * AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);

  /* set output pads */
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_SET_PADS_SHRINK_OUTPUT_PADS, 0);

  CU_ASSERT(audio->output_pads == AGS_AUDIO_TEST_SET_PADS_SHRINK_OUTPUT_PADS);

  /* verify pad count */
  channel = audio->output;
  
  for(i = 0; channel != audio->output->next_pad; i++){
    current = channel;
    
    for(j = 0; current != NULL; j++){
      current = current->next_pad;
    }
    
    CU_ASSERT(j == AGS_AUDIO_TEST_SET_PADS_SHRINK_OUTPUT_PADS);
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);

  /* verify line count */
  channel = audio->output;
  
  for(i = 0; channel != NULL; i++){
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_SHRINK_OUTPUT_PADS * AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);
}

void ags_audio_test_set_samplerate()
{
  //TODO:JK: implement me
}

void ags_audio_test_set_buffer_size()
{
  //TODO:JK: implement me
}

void ags_audio_test_set_format()
{
  //TODO:JK: implement me
}

void ags_audio_test_set_sequence_length()
{
  //TODO:JK: implement me
}

void
ags_audio_test_add_audio_connection()
{
  //TODO:JK: implement me
}

void
ags_audio_test_remove_audio_connection()
{
  //TODO:JK: implement me
}

void
ags_audio_test_add_preset()
{
  //TODO:JK: implement me
}

void
ags_audio_test_remove_preset()
{
  //TODO:JK: implement me
}

void
ags_audio_test_add_notation()
{
  //TODO:JK: implement me
}

void
ags_audio_test_remove_notation()
{
  //TODO:JK: implement me
}

void
ags_audio_test_add_automation()
{
  //TODO:JK: implement me
}

void
ags_audio_test_remove_automation()
{
  //TODO:JK: implement me
}

void
ags_audio_test_add_recall_id()
{
  AgsAudio *audio;
  AgsRecallID *recall_id;
  
  /* instantiate audio */
  audio = ags_audio_new(devout);

  /* instantiate recall */
  recall_id = ags_recall_id_new(NULL);

  /* add to audio */
  ags_audio_add_recall_id(audio,
			  recall_id);
  
  /* assert to be in audio->recall_id */
  CU_ASSERT(g_list_find(audio->recall_id,
			recall_id) != NULL);
}

void
ags_audio_test_remove_recall_id()
{
  //TODO:JK: implement me
}

void
ags_audio_test_add_recycling_context()
{
  AgsAudio *audio;
  AgsRecyclingContext *recycling_context;
  
  /* instantiate audio */
  audio = ags_audio_new(devout);

  /* instantiate recall */
  recycling_context = ags_recycling_context_new(0);

  /* add to audio */
  ags_audio_add_recycling_context(audio,
				  recycling_context);
  
  /* assert to be in audio->recycling_context */
  CU_ASSERT(g_list_find(audio->recycling_context,
			recycling_context) != NULL);
}

void
ags_audio_test_remove_recycling_context()
{
  //TODO:JK: implement me
}

void
ags_audio_test_add_recall_container()
{
  AgsAudio *audio;
  AgsRecallContainer *recall_container;
  
  /* instantiate audio */
  audio = ags_audio_new(devout);

  /* instantiate recall */
  recall_container = ags_recall_container_new();

  /* add to audio */
  ags_audio_add_recall_container(audio,
				 recall_container);
  
  /* assert to be in audio->recall_container */
  CU_ASSERT(g_list_find(audio->recall_container,
			recall_container) != NULL);
}

void
ags_audio_test_remove_recall_container()
{
  //TODO:JK: implement me
}

void
ags_audio_test_add_recall()
{
  AgsAudio *audio;
  AgsRecall *recall;

  /* instantiate audio */
  audio = ags_audio_new(devout);

  /* instantiate recall */
  recall = ags_recall_new();

  /* add recall to audio */
  ags_audio_add_recall(audio,
		       recall,
		       TRUE);

  /* assert to be in audio->play */
  CU_ASSERT(g_list_find(audio->play,
			recall) != NULL);

  /* instantiate recall */
  recall = ags_recall_new();

  /* add recall to audio */
  ags_audio_add_recall(audio,
		       recall,
		       FALSE);

  /* assert to be in audio->recall */
  CU_ASSERT(g_list_find(audio->recall,
			recall) != NULL);
}

void
ags_audio_test_remove_recall()
{
  //TODO:JK: implement me
}

void
ags_audio_test_init_run()
{
  //TODO:JK: implement me
}

void
ags_audio_test_duplicate_recall()
{
  AgsAudio *audio;
  AgsRecall *recall;
  AgsRecall *recall_audio_run;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  AgsRecallID *recall_id;
  
  /* instantiate audio */
  audio = ags_audio_new(devout);

  /* case 1: playback recall */
  recall = ags_recall_new();
  recall->flags |= (AGS_RECALL_TEMPLATE);
  recall->sound_scope = AGS_SOUND_SCOPE_NOTATION;
  recall->ability_flags = AGS_SOUND_ABILITY_NOTATION;
  ags_audio_add_recall(audio,
		       recall,
		       TRUE);
  
  recall_audio_run = ags_recall_audio_run_new();
  recall_audio_run->flags |= (AGS_RECALL_TEMPLATE);
  recall_audio_run->sound_scope = AGS_SOUND_SCOPE_NOTATION;
  recall_audio_run->ability_flags = AGS_SOUND_ABILITY_NOTATION;
  ags_audio_add_recall(audio,
		       recall_audio_run,
		       TRUE);
  
  /* assert inital count */
  CU_ASSERT(g_list_length(audio->play) == 2);
  CU_ASSERT(g_list_length(audio->recall) == 0);

  /* instantiate recycling context and recall id */
  recycling_context = ags_recycling_context_new(0);

  recall_id = ags_recall_id_new(NULL);
  recall_id->sound_scope = AGS_SOUND_SCOPE_NOTATION;
  g_object_set(recall_id,
	       "recycling-context", recycling_context,
	       NULL);

  /* duplicate recall */
  ags_audio_duplicate_recall(audio,
			     recall_id,
			     0, 0,
			     0);

  CU_ASSERT(g_list_length(audio->play) == 4);
  CU_ASSERT(g_list_length(audio->recall) == 0);
  
  /* case 2: true recall */
  recall = ags_recall_new();
  recall->flags |= (AGS_RECALL_TEMPLATE);
  recall->sound_scope = AGS_SOUND_SCOPE_NOTATION;
  recall->ability_flags = AGS_SOUND_ABILITY_NOTATION;
  ags_audio_add_recall(audio,
		       recall,
		       FALSE);
  
  recall_audio_run = ags_recall_audio_run_new();
  recall_audio_run->flags |= (AGS_RECALL_TEMPLATE);
  recall_audio_run->sound_scope = AGS_SOUND_SCOPE_NOTATION;
  recall_audio_run->ability_flags = AGS_SOUND_ABILITY_NOTATION;
  ags_audio_add_recall(audio,
		       recall_audio_run,
		       FALSE);

  /* assert inital count */
  CU_ASSERT(g_list_length(audio->play) == 4);
  CU_ASSERT(g_list_length(audio->recall) == 2);
  
  /* instantiate recycling context and recall id */
  parent_recycling_context = ags_recycling_context_new(0);
  
  recycling_context = ags_recycling_context_new(0);
  g_object_set(recycling_context,
	       "parent", parent_recycling_context,
	       NULL);

  recall_id = ags_recall_id_new(NULL);
  recall_id->sound_scope = AGS_SOUND_SCOPE_NOTATION;
  g_object_set(recall_id,
	       "recycling-context", recycling_context,
	       NULL);

  /* duplicate recall */
  ags_audio_duplicate_recall(audio,
			     recall_id,
			     0, 0,
			     0);

  CU_ASSERT(g_list_length(audio->play) == 4);
  CU_ASSERT(g_list_length(audio->recall) == 4);
}

void
ags_audio_test_is_playing()
{
  //TODO:JK: implement me
}

void
ags_audio_test_play()
{
  //TODO:JK: implement me
}  

void
ags_audio_test_tact()
{
  //TODO:JK: implement me
}

void
ags_audio_test_done()
{
  //TODO:JK: implement me
}

void
ags_audio_test_cancel()
{
  //TODO:JK: implement me
}

void
ags_audio_test_remove()
{
  //TODO:JK: implement me
}

void
ags_audio_test_find_port()
{
  //TODO:JK: implement me
}

void
ags_audio_test_open_files()
{
  //TODO:JK: implement me
}

void
ags_audio_test_recursive_set_property()
{
  //TODO:JK: implement me
}

void
ags_audio_test_recursive_play_init()
{
  //TODO:JK: implement me
}

void
ags_audio_test_link_channel()
{
  AgsChannel *channel, *link;
  AgsChannel *output, *first_channel;

  guint i;

  GError *error;
  
  /* audio - master */
  test_link_channel.master = ags_audio_new(devout);
  test_link_channel.master->flags |= AGS_AUDIO_ASYNC;
  
  ags_audio_set_audio_channels(test_link_channel.master,
			       AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(test_link_channel.master,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_INPUT_PADS, 0);
  ags_audio_set_pads(test_link_channel.master,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_OUTPUT_PADS, 0);

  /* audio - slave 0 */
  test_link_channel.slave_0 = ags_audio_new(devout);
  test_link_channel.slave_0->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
				       AGS_AUDIO_ASYNC);
  
  ags_audio_set_audio_channels(test_link_channel.slave_0,
			       AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_0_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(test_link_channel.slave_0,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_0_INPUT_PADS, 0);
  ags_audio_set_pads(test_link_channel.slave_0,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_0_OUTPUT_PADS, 0);

  /* audio - slave 1 */
  test_link_channel.slave_1 = ags_audio_new(devout);
  test_link_channel.slave_1->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
				       AGS_AUDIO_ASYNC);
  
  ags_audio_set_audio_channels(test_link_channel.slave_1,
			       AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_1_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(test_link_channel.slave_1,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_1_INPUT_PADS, 0);
  ags_audio_set_pads(test_link_channel.slave_1,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_1_OUTPUT_PADS, 0);

  /* audio - slave 2 */
  test_link_channel.slave_2 = ags_audio_new(devout);
  test_link_channel.slave_2->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
				       AGS_AUDIO_ASYNC);
  
  ags_audio_set_audio_channels(test_link_channel.slave_2,
			       AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_2_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(test_link_channel.slave_2,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_2_INPUT_PADS, 0);
  ags_audio_set_pads(test_link_channel.slave_2,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_2_OUTPUT_PADS, 0);

  /* setup link master to slave_0 */
  /* connect callback */
  channel = test_link_channel.master->input;
  link = test_link_channel.slave_0->output;
  
  for(i = 0;
      i < AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_0_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_AUDIO_CHANNELS;
      i++){
    //TODO:JK: implement me
    /*
    g_signal_connect(G_OBJECT(channel), "set-link",
		     G_CALLBACK(ags_audio_test_set_link_callback), test_link_channel.master);

    g_signal_connect(G_OBJECT(link), "set-link",
		     G_CALLBACK(ags_audio_test_set_link_callback), test_link_channel.slave_0);
    */
    
    channel = channel->next;
    link = link->next;
  }

  /* set link */
  channel = test_link_channel.master->input;
  link = test_link_channel.slave_0->output;
  
  for(i = 0;
      i < AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_0_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_AUDIO_CHANNELS;
      i++){
    error = NULL;
    ags_channel_set_link(channel, link,
			 &error);

    /* assert link set */
    CU_ASSERT(error == NULL);
    CU_ASSERT(channel->link == link);
    CU_ASSERT(link->link == channel);

    /* check recycling */
    CU_ASSERT(channel->first_recycling == link->first_recycling);
    CU_ASSERT(channel->last_recycling == link->last_recycling);

    /* iterate */
    channel = channel->next;
    link = link->next;
  }

  /* check output recycling */
  output = test_link_channel.master->output;
  first_channel = 
    link = test_link_channel.slave_0->output;
  
  for(i = 0;
      i < AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_0_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_AUDIO_CHANNELS;
      i++){
    CU_ASSERT(output->first_recycling == link->first_recycling);
    CU_ASSERT(output->last_recycling == link->last_recycling);
  }  
  
  /* setup link master to slave_1 */
  /* connect callback */
  channel = ags_channel_pad_nth(test_link_channel.master->input,
				1);
  link = test_link_channel.slave_1->output;
  
  for(i = 0;
      i < AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_1_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_AUDIO_CHANNELS;
      i++){
    //TODO:JK: implement me
    /*
    g_signal_connect(G_OBJECT(channel), "set-link",
		     G_CALLBACK(ags_audio_test_set_link_callback), test_link_channel.master);

    g_signal_connect(G_OBJECT(link), "set-link",
		     G_CALLBACK(ags_audio_test_set_link_callback), test_link_channel.slave_1);
    */
    
    channel = channel->next;
    link = link->next;
  }

  /* set link */
  channel = ags_channel_pad_nth(test_link_channel.master->input,
				1);
  link = test_link_channel.slave_1->output;
  
  for(i = 0;
      i < AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_1_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_AUDIO_CHANNELS;
      i++){
    error = NULL;
    ags_channel_set_link(channel, link,
			 &error);

    /* assert link set */
    CU_ASSERT(error == NULL);
    CU_ASSERT(channel->link == link);
    CU_ASSERT(link->link == channel);

    /* check recycling */
    CU_ASSERT(channel->first_recycling == link->first_recycling);
    CU_ASSERT(channel->last_recycling == link->last_recycling);

    /* iterate */
    channel = channel->next;
    link = link->next;
  }

  /* check output recycling */
  output = test_link_channel.master->output;
  first_channel = test_link_channel.slave_0->output;
  link = test_link_channel.slave_1->output;
  
  for(i = 0;
      i < AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_1_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_AUDIO_CHANNELS;
      i++){
    CU_ASSERT(output->first_recycling == first_channel->first_recycling);
    CU_ASSERT(output->last_recycling == link->last_recycling);

    output = output->next;
    first_channel = first_channel->next;
    link = link->next;
  }  
  
  /* setup link master to slave_2 */
  /* connect callback */
  channel = ags_channel_pad_nth(test_link_channel.master->input,
				2);
  link = test_link_channel.slave_2->output;
  
  for(i = 0;
      i < AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_2_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_AUDIO_CHANNELS;
      i++){
    //TODO:JK: implement me
    /*
    g_signal_connect(G_OBJECT(channel), "set-link",
		     G_CALLBACK(ags_audio_test_set_link_callback), test_link_channel.master);

    g_signal_connect(G_OBJECT(link), "set-link",
		     G_CALLBACK(ags_audio_test_set_link_callback), test_link_channel.slave_2);
    */
    
    channel = channel->next;
    link = link->next;
  }

  /* set link */
  channel = ags_channel_pad_nth(test_link_channel.master->input,
				2);
  link = test_link_channel.slave_2->output;
  
  for(i = 0;
      i < AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_2_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_AUDIO_CHANNELS;
      i++){
    error = NULL;
    ags_channel_set_link(channel, link,
			 &error);

    /* assert link set */
    CU_ASSERT(error == NULL);
    CU_ASSERT(channel->link == link);
    CU_ASSERT(link->link == channel);

    /* check recycling */
    CU_ASSERT(channel->first_recycling == link->first_recycling);
    CU_ASSERT(channel->last_recycling == link->last_recycling);

    /* iterate */
    channel = channel->next;
    link = link->next;
  }

  /* check output recycling */
  output = test_link_channel.master->output;
  first_channel = test_link_channel.slave_0->output;
  link = test_link_channel.slave_2->output;
  
  for(i = 0;
      i < AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_2_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_AUDIO_CHANNELS;
      i++){
    CU_ASSERT(output->first_recycling == first_channel->first_recycling);
    CU_ASSERT(output->last_recycling == link->last_recycling);

    output = output->next;
    first_channel = first_channel->next;
    link = link->next;
  }  
}

void
ags_audio_test_finalize_linked_channel()
{
  AgsChannel *channel, *link;
  AgsChannel *output, *current, *last_channel;

  guint i;

  GError *error;
  
  /* audio - master */
  test_finalize_linked_channel.master = ags_audio_new(devout);
  test_finalize_linked_channel.master->flags |= AGS_AUDIO_ASYNC;
  
  ags_audio_set_audio_channels(test_finalize_linked_channel.master,
			       AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(test_finalize_linked_channel.master,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_INPUT_PADS, 0);
  ags_audio_set_pads(test_finalize_linked_channel.master,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_OUTPUT_PADS, 0);

  /* audio - slave 0 */
  test_finalize_linked_channel.slave_0 = ags_audio_new(devout);
  test_finalize_linked_channel.slave_0->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
						  AGS_AUDIO_ASYNC);
  
  ags_audio_set_audio_channels(test_finalize_linked_channel.slave_0,
			       AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_0_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(test_finalize_linked_channel.slave_0,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_0_INPUT_PADS, 0);
  ags_audio_set_pads(test_finalize_linked_channel.slave_0,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_0_OUTPUT_PADS, 0);

  /* audio - slave 1 */
  test_finalize_linked_channel.slave_1 = ags_audio_new(devout);
  test_finalize_linked_channel.slave_1->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
						  AGS_AUDIO_ASYNC);
  
  ags_audio_set_audio_channels(test_finalize_linked_channel.slave_1,
			       AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_1_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(test_finalize_linked_channel.slave_1,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_1_INPUT_PADS, 0);
  ags_audio_set_pads(test_finalize_linked_channel.slave_1,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_1_OUTPUT_PADS, 0);

  /* audio - slave 2 */
  test_finalize_linked_channel.slave_2 = ags_audio_new(devout);
  test_finalize_linked_channel.slave_2->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
						  AGS_AUDIO_ASYNC);
  
  ags_audio_set_audio_channels(test_finalize_linked_channel.slave_2,
			       AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_2_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(test_finalize_linked_channel.slave_2,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_2_INPUT_PADS, 0);
  ags_audio_set_pads(test_finalize_linked_channel.slave_2,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_2_OUTPUT_PADS, 0);

  /* setup link master to slave_0 */
  /* set link */
  channel = test_finalize_linked_channel.master->input;
  link = test_finalize_linked_channel.slave_0->output;
  
  for(i = 0;
      i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_0_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_AUDIO_CHANNELS;
      i++){
    error = NULL;
    ags_channel_set_link(channel, link,
			 &error);

    /* iterate */
    channel = channel->next;
    link = link->next;
  }

  /* setup link master to slave_1 */
  /* set link */
  channel = ags_channel_pad_nth(test_finalize_linked_channel.master->input,
				1);
  link = test_finalize_linked_channel.slave_1->output;
  
  for(i = 0;
      i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_1_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_AUDIO_CHANNELS;
      i++){
    error = NULL;
    ags_channel_set_link(channel, link,
			 &error);

    /* iterate */
    channel = channel->next;
    link = link->next;
  }
  
  /* setup link master to slave_2 */
  /* set link */
  channel = ags_channel_pad_nth(test_finalize_linked_channel.master->input,
				2);
  link = test_finalize_linked_channel.slave_2->output;
  
  for(i = 0;
      i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_2_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_AUDIO_CHANNELS;
      i++){
    error = NULL;
    ags_channel_set_link(channel, link,
			 &error);
    
    /* iterate */
    channel = channel->next;
    link = link->next;
  }

  /* asserts */
  /* unset link */
  channel = test_finalize_linked_channel.master->input;

  ags_audio_set_pads(test_finalize_linked_channel.slave_0,
		     AGS_TYPE_INPUT,
		     0, 0);
  ags_audio_set_pads(test_finalize_linked_channel.slave_0,
		     AGS_TYPE_OUTPUT,
		     0, 0);

  g_object_unref(test_finalize_linked_channel.slave_0);
  
  for(i = 0; i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_AUDIO_CHANNELS; i++){
    /* assert link set */
    CU_ASSERT(channel->link == NULL);

    /* check recycling */
    CU_ASSERT(channel->first_recycling == NULL);
    CU_ASSERT(channel->last_recycling == NULL);

    /* iterate */
    channel = channel->next;
  }

  /* check output recycling */
  output = test_finalize_linked_channel.master->output;
  current = test_finalize_linked_channel.slave_1->output;
  
  for(i = 0; i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_1_AUDIO_CHANNELS;
      i++){
    CU_ASSERT(output->first_recycling == current->first_recycling);

    output = output->next;
    current = current->next;
  }  

  output = test_finalize_linked_channel.master->output;
  last_channel = test_finalize_linked_channel.slave_2->output;
  
  for(i = 0; i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_2_AUDIO_CHANNELS;
      i++){
    CU_ASSERT(output->last_recycling == last_channel->last_recycling);

    output = output->next;
    last_channel = last_channel->next;
  }  
  
  /* unset link */
  channel = ags_channel_pad_nth(test_finalize_linked_channel.master->input,
				1);
  
  ags_audio_set_pads(test_finalize_linked_channel.slave_1,
		     AGS_TYPE_INPUT,
		     0, 0);
  ags_audio_set_pads(test_finalize_linked_channel.slave_1,
		     AGS_TYPE_OUTPUT,
		     0, 0);

  g_object_unref(test_finalize_linked_channel.slave_1);
  
  for(i = 0; i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_AUDIO_CHANNELS; i++){
    /* assert link set */
    CU_ASSERT(channel->link == NULL);

    /* check recycling */
    CU_ASSERT(channel->first_recycling == NULL);
    CU_ASSERT(channel->last_recycling == NULL);

    /* iterate */
    channel = channel->next;
  }

  /* check output recycling */
  output = test_finalize_linked_channel.master->output;
  last_channel = test_finalize_linked_channel.slave_2->output;
  
  for(i = 0;
      i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_1_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_AUDIO_CHANNELS;
      i++){
    /* check recycling */
    CU_ASSERT(output->first_recycling == last_channel->first_recycling);
    CU_ASSERT(output->last_recycling == last_channel->last_recycling);

    output = output->next;
    last_channel = last_channel->next;
  }  

  /* unset link */
  channel = ags_channel_pad_nth(test_finalize_linked_channel.master->input,
				2);

  ags_audio_set_pads(test_finalize_linked_channel.slave_2,
		     AGS_TYPE_INPUT,
		     0, 0);
  ags_audio_set_pads(test_finalize_linked_channel.slave_2,
		     AGS_TYPE_OUTPUT,
		     0, 0);

  g_object_unref(test_finalize_linked_channel.slave_2);

  for(i = 0;
      i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_2_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_AUDIO_CHANNELS;
      i++){
    /* assert link unset */
    CU_ASSERT(channel->link == NULL);

    /* check recycling */
    CU_ASSERT(channel->first_recycling == NULL);
    CU_ASSERT(channel->last_recycling == NULL);

    /* iterate */
    channel = channel->next;
  }

  /* check output recycling */
  output = test_finalize_linked_channel.master->output;
  
  for(i = 0;
      i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_2_AUDIO_CHANNELS &&
	i < AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_AUDIO_CHANNELS;
      i++){
    CU_ASSERT(output->first_recycling == NULL);
    CU_ASSERT(output->last_recycling == NULL);

    output = output->next;
  }  
}

void
ags_audio_test_init_recall()
{
  AgsAudio *audio;
  AgsRecall *recall;
  AgsRecall *recall_audio_run;
  AgsRecyclingContext *recycling_context;
  AgsRecallID *recall_id;
    
  /* instantiate audio */
  audio = ags_audio_new(devout);

  /* instantiate recalls */
  recall = ags_recall_new();
  recall->ability_flags = AGS_SOUND_ABILITY_NOTATION;
  ags_audio_add_recall(audio,
		       recall,
		       TRUE);

  g_signal_connect(G_OBJECT(recall), "run-init-pre",
		   G_CALLBACK(ags_audio_test_init_recall_callback), NULL);
  
  recall_audio_run = ags_recall_audio_run_new();
  recall_audio_run->ability_flags = AGS_SOUND_ABILITY_NOTATION;
  ags_audio_add_recall(audio,
		       recall_audio_run,
		       TRUE);

  g_signal_connect(G_OBJECT(recall_audio_run), "run-init-pre",
		   G_CALLBACK(ags_audio_test_init_recall_callback), NULL);
  
  /* instantiate recycling context and recall id */
  recycling_context = ags_recycling_context_new(0);

  recall_id = ags_recall_id_new(NULL);
  recall_id->sound_scope = AGS_SOUND_SCOPE_NOTATION;
  g_object_set(recall_id,
	       "recycling-context", recycling_context,
	       NULL);
  
  /* setup recalls */
  g_object_set(recall,
	       "recall-id", recall_id,
	       NULL);

  g_object_set(recall_audio_run,
	       "recall-id", recall_id,
	       NULL);
  
  /* init recall */
  ags_audio_init_recall(audio,
			recall_id, AGS_SOUND_STAGING_RUN_INIT_PRE);
  
  CU_ASSERT(test_init_recall_callback_hits_count == 1);
}

void
ags_audio_test_resolve_recall()
{
  AgsAudio *audio;
  AgsRecall *master_recall_audio_run;
  AgsRecall  *slave_recall_audio_run;
  AgsRecyclingContext *recycling_context;
  AgsRecallID *recall_id;
  
  /* instantiate audio */
  audio = ags_audio_new(devout);

  /* instantiate recalls */
  slave_recall_audio_run = ags_recall_audio_run_new();
  ags_audio_add_recall(audio,
		       slave_recall_audio_run,
		       TRUE);

  g_signal_connect(G_OBJECT(slave_recall_audio_run), "resolve-dependency",
		   G_CALLBACK(ags_audio_test_resolve_recall_callback), NULL);
  
  /* instantiate recycling context and recall id */
  recycling_context = ags_recycling_context_new(0);

  recall_id = ags_recall_id_new(NULL);
  recall_id->sound_scope = AGS_SOUND_SCOPE_NOTATION;
  g_object_set(recall_id,
	       "recycling-context", recycling_context,
	       NULL);
  
  /* setup recalls */
  g_object_set(slave_recall_audio_run,
	       "recall-id", recall_id,
	       NULL);

  /* resolve recall */
  ags_audio_resolve_recall(audio,
			   recall_id);
  
  CU_ASSERT(test_resolve_recall_callback_hits_count == 1);
}

void
ags_audio_test_finalize_stub(GObject *gobject)
{
  audio_test_finalized = TRUE;
}

void
ags_audio_test_set_link_callback(AgsChannel *channel, AgsChannel *link,
				 GError **error,
				 AgsAudio *audio)
{
  //TODO:JK: implement me
}

void
ags_audio_test_init_recall_callback(AgsRecall *recall,
				    gpointer data)
{
  g_message("init recall");
  
  test_init_recall_callback_hits_count++;
}

void
ags_audio_test_resolve_recall_callback(AgsRecall *recall,
				       gpointer data)
{
  g_message("resolve recall");
  
  test_resolve_recall_callback_hits_count++;
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
  pSuite = CU_add_suite("AgsAudioTest", ags_audio_test_init_suite, ags_audio_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsAudio dispose", ags_audio_test_dispose) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio finalize", ags_audio_test_finalize) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio set pads", ags_audio_test_set_pads) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio set audio channels", ags_audio_test_set_audio_channels) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio link channel", ags_audio_test_link_channel) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio finalize linked channel", ags_audio_test_finalize_linked_channel) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio add recall", ags_audio_test_add_recall) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio add recall container", ags_audio_test_add_recall_container) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio recall id", ags_audio_test_add_recall_id) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio recycling context", ags_audio_test_add_recycling_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio duplicate recall", ags_audio_test_duplicate_recall) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio initialize recall", ags_audio_test_init_recall) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio resolve recall", ags_audio_test_resolve_recall) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

