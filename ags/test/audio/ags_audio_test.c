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

#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recycling_context.h>

int ags_audio_test_init_suite();
int ags_audio_test_clean_suite();

void ags_audio_test_set_pads();
void ags_audio_test_set_audio_channels();
void ags_audio_test_link_channel();
void ags_audio_test_finalize_linked_channel();
void ags_audio_test_add_recall();
void ags_audio_test_add_recall_container();
void ags_audio_test_add_recall_id();
void ags_audio_test_add_recycling_context();
void ags_audio_test_duplicate_recall();
void ags_audio_test_init_recall();
void ags_audio_test_resolve_recall();

void ags_audio_test_set_link_callback(AgsChannel *channel, AgsChannel *link,
				      GError **error,
				      AgsAudio *audio);
void ags_audio_test_init_recall_callback(AgsRecall *recall,
					 gpointer data);
void ags_audio_test_resolve_recall_callback(AgsRecall *recall,
					    gpointer data);

#define AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS (2)
#define AGS_AUDIO_TEST_SET_PADS_INPUT_PADS (12)
#define AGS_AUDIO_TEST_SET_PADS_OUTPUT_PADS (5)

#define AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS (3)
#define AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_INPUT_PADS (1)
#define AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_OUTPUT_PADS (7)

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

AgsDevout *devout;

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
  devout = ags_devout_new(NULL);

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
ags_audio_test_set_pads()
{
  AgsAudio *audio;
  AgsChannel *channel, *current;

  guint i, j;
  
  /* instantiate */
  audio = ags_audio_new(AGS_SOUNDCARD(devout));

  CU_ASSERT(audio != NULL);

  /* set audio channels */
  audio->audio_channels = AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS;

  /* set input pads */
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_SET_PADS_INPUT_PADS);

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

  /* verify line count */
  channel = audio->input;
  
  for(i = 0; channel != NULL; i++){
    channel = channel->next;
  }

  CU_ASSERT(i == AGS_AUDIO_TEST_SET_PADS_INPUT_PADS * AGS_AUDIO_TEST_SET_PADS_AUDIO_CHANNELS);

  /* set output pads */
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_SET_PADS_OUTPUT_PADS);

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
}

void
ags_audio_test_set_audio_channels()
{
  AgsAudio *audio;
  AgsChannel *channel, *current;

  guint i, j;
  
  /* instantiate */
  audio = ags_audio_new(AGS_SOUNDCARD(devout));

  CU_ASSERT(audio != NULL);

  /* set pads */
  audio->output_pads = AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_OUTPUT_PADS;
  audio->input_pads = AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_INPUT_PADS;

  /* set audio channels */
  ags_audio_set_audio_channels(audio,
			       AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS);

  CU_ASSERT(audio->audio_channels == AGS_AUDIO_TEST_SET_AUDIO_CHANNELS_AUDIO_CHANNELS);

  /* verify pad count */
  channel = audio->input;
  
  for(i = 0; channel != audio->input->next_pad; i++){
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
  
  for(i = 0; channel != audio->output->next_pad; i++){
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
ags_audio_test_link_channel()
{
  AgsChannel *channel, *link;
  AgsChannel *output, *first_channel;

  guint i;

  GError *error;
  
  /* audio - master */
  test_link_channel.master = ags_audio_new(AGS_SOUNDCARD(devout));
  test_link_channel.master->flags |= AGS_AUDIO_ASYNC;
  
  ags_audio_set_audio_channels(test_link_channel.master,
			       AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_AUDIO_CHANNELS);
  
  ags_audio_set_pads(test_link_channel.master,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_INPUT_PADS);
  ags_audio_set_pads(test_link_channel.master,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_LINK_CHANNEL_MASTER_OUTPUT_PADS);

  /* audio - slave 0 */
  test_link_channel.slave_0 = ags_audio_new(AGS_SOUNDCARD(devout));
  test_link_channel.slave_0->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
				       AGS_AUDIO_ASYNC);
  
  ags_audio_set_audio_channels(test_link_channel.slave_0,
			       AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_0_AUDIO_CHANNELS);
  
  ags_audio_set_pads(test_link_channel.slave_0,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_0_INPUT_PADS);
  ags_audio_set_pads(test_link_channel.slave_0,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_0_OUTPUT_PADS);

  /* audio - slave 1 */
  test_link_channel.slave_1 = ags_audio_new(AGS_SOUNDCARD(devout));
  test_link_channel.slave_1->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
				       AGS_AUDIO_ASYNC);
  
  ags_audio_set_audio_channels(test_link_channel.slave_1,
			       AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_1_AUDIO_CHANNELS);
  
  ags_audio_set_pads(test_link_channel.slave_1,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_1_INPUT_PADS);
  ags_audio_set_pads(test_link_channel.slave_1,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_1_OUTPUT_PADS);

  /* audio - slave 2 */
  test_link_channel.slave_2 = ags_audio_new(AGS_SOUNDCARD(devout));
  test_link_channel.slave_2->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
				       AGS_AUDIO_ASYNC);
  
  ags_audio_set_audio_channels(test_link_channel.slave_2,
			       AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_2_AUDIO_CHANNELS);
  
  ags_audio_set_pads(test_link_channel.slave_2,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_2_INPUT_PADS);
  ags_audio_set_pads(test_link_channel.slave_2,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_LINK_CHANNEL_SLAVE_2_OUTPUT_PADS);

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
    g_signal_connect(G_OBJECT(channel), "set-link\0",
		     G_CALLBACK(ags_audio_test_set_link_callback), test_link_channel.master);

    g_signal_connect(G_OBJECT(link), "set-link\0",
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
    g_signal_connect(G_OBJECT(channel), "set-link\0",
		     G_CALLBACK(ags_audio_test_set_link_callback), test_link_channel.master);

    g_signal_connect(G_OBJECT(link), "set-link\0",
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
    g_signal_connect(G_OBJECT(channel), "set-link\0",
		     G_CALLBACK(ags_audio_test_set_link_callback), test_link_channel.master);

    g_signal_connect(G_OBJECT(link), "set-link\0",
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
  test_finalize_linked_channel.master = ags_audio_new(AGS_SOUNDCARD(devout));
  test_finalize_linked_channel.master->flags |= AGS_AUDIO_ASYNC;
  
  ags_audio_set_audio_channels(test_finalize_linked_channel.master,
			       AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_AUDIO_CHANNELS);
  
  ags_audio_set_pads(test_finalize_linked_channel.master,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_INPUT_PADS);
  ags_audio_set_pads(test_finalize_linked_channel.master,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_MASTER_OUTPUT_PADS);

  /* audio - slave 0 */
  test_finalize_linked_channel.slave_0 = ags_audio_new(AGS_SOUNDCARD(devout));
  test_finalize_linked_channel.slave_0->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
						  AGS_AUDIO_ASYNC);
  
  ags_audio_set_audio_channels(test_finalize_linked_channel.slave_0,
			       AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_0_AUDIO_CHANNELS);
  
  ags_audio_set_pads(test_finalize_linked_channel.slave_0,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_0_INPUT_PADS);
  ags_audio_set_pads(test_finalize_linked_channel.slave_0,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_0_OUTPUT_PADS);

  /* audio - slave 1 */
  test_finalize_linked_channel.slave_1 = ags_audio_new(AGS_SOUNDCARD(devout));
  test_finalize_linked_channel.slave_1->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
						  AGS_AUDIO_ASYNC);
  
  ags_audio_set_audio_channels(test_finalize_linked_channel.slave_1,
			       AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_1_AUDIO_CHANNELS);
  
  ags_audio_set_pads(test_finalize_linked_channel.slave_1,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_1_INPUT_PADS);
  ags_audio_set_pads(test_finalize_linked_channel.slave_1,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_1_OUTPUT_PADS);

  /* audio - slave 2 */
  test_finalize_linked_channel.slave_2 = ags_audio_new(AGS_SOUNDCARD(devout));
  test_finalize_linked_channel.slave_2->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
						  AGS_AUDIO_ASYNC);
  
  ags_audio_set_audio_channels(test_finalize_linked_channel.slave_2,
			       AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_2_AUDIO_CHANNELS);
  
  ags_audio_set_pads(test_finalize_linked_channel.slave_2,
		     AGS_TYPE_INPUT,
		     AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_2_INPUT_PADS);
  ags_audio_set_pads(test_finalize_linked_channel.slave_2,
		     AGS_TYPE_OUTPUT,
		     AGS_AUDIO_TEST_FINALIZE_LINKED_CHANNEL_SLAVE_2_OUTPUT_PADS);

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
		     0);
  ags_audio_set_pads(test_finalize_linked_channel.slave_0,
		     AGS_TYPE_OUTPUT,
		     0);

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
		     0);
  ags_audio_set_pads(test_finalize_linked_channel.slave_1,
		     AGS_TYPE_OUTPUT,
		     0);

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
		     0);
  ags_audio_set_pads(test_finalize_linked_channel.slave_2,
		     AGS_TYPE_OUTPUT,
		     0);

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
ags_audio_test_add_recall()
{
  AgsAudio *audio;
  AgsRecall *recall;

  /* instantiate audio */
  audio = ags_audio_new(AGS_SOUNDCARD(devout));

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
ags_audio_test_add_recall_container()
{
  AgsAudio *audio;
  AgsRecallContainer *recall_container;
  
  /* instantiate audio */
  audio = ags_audio_new(AGS_SOUNDCARD(devout));

  /* instantiate recall */
  recall_container = ags_recall_container_new();

  /* add to audio */
  ags_audio_add_recall_container(audio,
				 recall_container);
  
  /* assert to be in audio->recall_container */
  CU_ASSERT(g_list_find(audio->container,
			recall_container) != NULL);
}

void
ags_audio_test_add_recall_id()
{
  AgsAudio *audio;
  AgsRecallID *recall_id;
  
  /* instantiate audio */
  audio = ags_audio_new(AGS_SOUNDCARD(devout));

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
ags_audio_test_add_recycling_context()
{
  AgsAudio *audio;
  AgsRecyclingContext *recycling_context;
  
  /* instantiate audio */
  audio = ags_audio_new(AGS_SOUNDCARD(devout));

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
ags_audio_test_duplicate_recall()
{
  AgsAudio *audio;
  AgsRecall *recall;
  AgsRecall *recall_audio_run;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  AgsRecallID *recall_id;
  
  /* instantiate audio */
  audio = ags_audio_new(AGS_SOUNDCARD(devout));

  /* case 1: playback recall */
  recall = ags_recall_new();
  ags_audio_add_recall(audio,
		       recall,
		       TRUE);
  
  recall_audio_run = ags_recall_audio_run_new();
  ags_audio_add_recall(audio,
		       recall_audio_run,
		       TRUE);
  
  /* assert inital count */
  CU_ASSERT(g_list_length(audio->play) == 2);
  CU_ASSERT(g_list_length(audio->recall) == 0);

  /* instantiate recycling context and recall id */
  recycling_context = ags_recycling_context_new(0);

  recall_id = ags_recall_id_new(NULL);
  g_object_set(recall_id,
	       "recycling-context\0", recycling_context,
	       NULL);

  /* duplicate recall */
  ags_audio_duplicate_recall(audio,
			     recall_id);

  CU_ASSERT(g_list_length(audio->play) == 4);
  CU_ASSERT(g_list_length(audio->recall) == 0);
  
  /* case 2: true recall */
  recall = ags_recall_new();
  ags_audio_add_recall(audio,
		       recall,
		       FALSE);
  
  recall_audio_run = ags_recall_audio_run_new();
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
	       "parent\0", parent_recycling_context,
	       NULL);

  recall_id = ags_recall_id_new(NULL);
  g_object_set(recall_id,
	       "recycling-context\0", recycling_context,
	       NULL);

  /* duplicate recall */
  ags_audio_duplicate_recall(audio,
			     recall_id);

  CU_ASSERT(g_list_length(audio->play) == 4);
  CU_ASSERT(g_list_length(audio->recall) == 4);
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
  audio = ags_audio_new(AGS_SOUNDCARD(devout));

  /* instantiate recalls */
  recall = ags_recall_new();
  ags_audio_add_recall(audio,
		       recall,
		       TRUE);

  g_signal_connect(G_OBJECT(recall), "init-pre\0",
		   G_CALLBACK(ags_audio_test_init_recall_callback), NULL);
  
  recall_audio_run = ags_recall_audio_run_new();
  ags_audio_add_recall(audio,
		       recall_audio_run,
		       TRUE);

  g_signal_connect(G_OBJECT(recall_audio_run), "init-pre\0",
		   G_CALLBACK(ags_audio_test_init_recall_callback), NULL);
  
  /* instantiate recycling context and recall id */
  recycling_context = ags_recycling_context_new(0);

  recall_id = ags_recall_id_new(NULL);
  g_object_set(recall_id,
	       "recycling-context\0", recycling_context,
	       NULL);
  
  /* setup recalls */
  g_object_set(recall,
	       "recall-id\0", recall_id,
	       NULL);

  g_object_set(recall_audio_run,
	       "recall-id\0", recall_id,
	       NULL);
  
  /* init recall */
  ags_audio_init_recall(audio, 0,
			recall_id);
  
  CU_ASSERT(test_init_recall_callback_hits_count == 2);
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
  audio = ags_audio_new(AGS_SOUNDCARD(devout));

  /* instantiate recalls */
  slave_recall_audio_run = ags_recall_audio_run_new();
  ags_audio_add_recall(audio,
		       slave_recall_audio_run,
		       TRUE);

  g_signal_connect(G_OBJECT(slave_recall_audio_run), "resolve\0",
		   G_CALLBACK(ags_audio_test_resolve_recall_callback), NULL);
  
  /* instantiate recycling context and recall id */
  recycling_context = ags_recycling_context_new(0);

  recall_id = ags_recall_id_new(NULL);
  g_object_set(recall_id,
	       "recycling-context\0", recycling_context,
	       NULL);
  
  /* setup recalls */
  g_object_set(slave_recall_audio_run,
	       "recall-id\0", recall_id,
	       NULL);

  /* resolve recall */
  ags_audio_resolve_recall(audio,
			   recall_id);
  
  CU_ASSERT(test_resolve_recall_callback_hits_count == 1);
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
  test_init_recall_callback_hits_count++;
}

void
ags_audio_test_resolve_recall_callback(AgsRecall *recall,
				       gpointer data)
{
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
  pSuite = CU_add_suite("AgsAudioTest\0", ags_audio_test_init_suite, ags_audio_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsAudio set pads\0", ags_audio_test_set_pads) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio set audio channels\0", ags_audio_test_set_audio_channels) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio link channel\0", ags_audio_test_link_channel) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio finalize linked channel\0", ags_audio_test_finalize_linked_channel) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio add recall\0", ags_audio_test_add_recall) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio add recall container\0", ags_audio_test_add_recall_container) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio recall id\0", ags_audio_test_add_recall_id) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio recycling context\0", ags_audio_test_add_recycling_context) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio duplicate recall\0", ags_audio_test_duplicate_recall) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio initialize recall\0", ags_audio_test_init_recall) == NULL) ||
     (CU_add_test(pSuite, "test of AgsAudio resolve recall\0", ags_audio_test_resolve_recall) == NULL)){
      CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

