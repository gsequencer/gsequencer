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
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

int ags_recycling_test_init_suite();
int ags_recycling_test_clean_suite();

void ags_recycling_test_add_audio_signal();
void ags_recycling_test_remove_audio_signal();
void ags_recycling_test_create_audio_signal_with_defaults();
void ags_recycling_test_create_audio_signal_with_frame_count();
void ags_recycling_test_position();
void ags_recycling_test_find_next_channel();

#define AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_SAMPLERATE (44100)
#define AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_BUFFER_SIZE (944)
#define AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_FORMAT (AGS_AUDIO_BUFFER_UTIL_S16)
#define AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_FREQUENCY (440.0)
#define AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_FRAMES (AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_SAMPLERATE / \
								     AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_BUFFER_SIZE * \
								     440.0)

#define AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_SAMPLERATE (44100)
#define AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_BUFFER_SIZE (944)
#define AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_FORMAT (AGS_AUDIO_BUFFER_UTIL_S16)
#define AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_FREQUENCY (440.0)
#define AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_FRAMES (AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_SAMPLERATE / \
									AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_BUFFER_SIZE * \
									440.0)
#define AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_LAST_FRAME (AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_SAMPLERATE / \
									    AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_BUFFER_SIZE * \
									    440.0 - 1.0)

#define AGS_RECYCLING_TEST_POSITION_N_RECYCLING (16)
#define AGS_RECYCLING_TEST_FIND_NEXT_CHANNEL_N_CHANNEL (8)

AgsDevout *devout;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recycling_test_init_suite()
{ 
  devout = ags_devout_new(NULL);

  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_recycling_test_clean_suite()
{
  g_object_unref(devout);
  
  return(0);
}

void
ags_recycling_test_add_audio_signal()
{
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;
  
  /* instantiate recycling */
  recycling = ags_recycling_new(G_OBJECT(devout));

  /* instantiate audio signal */
  audio_signal = ags_audio_signal_new(G_OBJECT(devout),
				      recycling,
				      NULL);

  /* add audio signal */
  ags_recycling_add_audio_signal(recycling,
				 audio_signal);

  /* assert if audio signal available in recycling */
  CU_ASSERT(g_list_find(recycling->audio_signal,
			audio_signal) != NULL);
}

void
ags_recycling_test_remove_audio_signal()
{
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;
  
  /* instantiate recycling */
  recycling = ags_recycling_new(G_OBJECT(devout));

  /* instantiate audio signal */
  audio_signal = ags_audio_signal_new(G_OBJECT(devout),
				      recycling,
				      NULL);

  /* add audio signal */
  ags_recycling_add_audio_signal(recycling,
				 audio_signal);

  /* assert if audio signal available in recycling */
  CU_ASSERT(g_list_find(recycling->audio_signal,
			audio_signal) != NULL);

  /* add audio signal */
  ags_recycling_remove_audio_signal(recycling,
				    audio_signal);

  /* assert if no audio signal in recycling */
  CU_ASSERT(g_list_find(recycling->audio_signal,
			audio_signal) == NULL);
}

void
ags_recycling_test_create_audio_signal_with_defaults()
{
  AgsRecycling *recycling;
  AgsAudioSignal *template, *audio_signal;
  GList *stream, *template_stream;
  signed short *buffer;
  guint i, j;
  
  /* instantiate recycling */
  recycling = ags_recycling_new(G_OBJECT(devout));

  /* instantiate template audio signal */
  template = ags_audio_signal_new(G_OBJECT(devout),
				  recycling,
				  NULL);
  template->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
  g_object_set(G_OBJECT(template),
	       "samplerate\0", AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_SAMPLERATE,
	       "buffer-size\0", AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_BUFFER_SIZE,
	       "format\0", AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_FORMAT,
	       NULL);

  /* fill stream */
  stream = NULL;
  
  for(i = 0; i < AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_FRAMES;){
    buffer = (signed short *) malloc(template->buffer_size * sizeof(signed short));
    memset(buffer, 0, template->buffer_size * sizeof(signed short));
    
    for(j = 0;
	j < AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_BUFFER_SIZE &&
	  i + j < AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_FRAMES;
	j++){
      /* generate sin tone */
      buffer[j] = (signed short) (0xffff & (int) (32000.0 * (double) (sin ((double)(j) * 2.0 * M_PI * AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_FREQUENCY / (double) AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_SAMPLERATE))));
    }

    /* prepend buffer */
    stream = g_list_prepend(stream,
			    buffer);

    /* iterate */
    i += AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_BUFFER_SIZE;
  }

  template->stream_end = stream;
  template->stream_beginning = g_list_reverse(stream);
  
  /* add audio signal to recycling */
  ags_recycling_add_audio_signal(recycling,
				 template);

  /* instantiate audio signal */
  audio_signal = ags_audio_signal_new(G_OBJECT(devout),
				      recycling,
				      NULL);

  /* create defaults */
  ags_recycling_create_audio_signal_with_defaults(recycling,
						  template,
						  0.0, 0);

  /* assert audio signal */
  CU_ASSERT(audio_signal->samplerate == template->samplerate);
  CU_ASSERT(audio_signal->buffer_size == template->buffer_size);
  CU_ASSERT(audio_signal->format == template->format);

  CU_ASSERT(audio_signal->length == template->length);
  CU_ASSERT(audio_signal->first_frame == template->first_frame);
  CU_ASSERT(audio_signal->last_frame == template->last_frame);
  CU_ASSERT(audio_signal->loop_start == template->loop_start);
  CU_ASSERT(audio_signal->loop_end == template->loop_end);

  stream = audio_signal->stream_beginning;
  template_stream = template->stream_beginning;
  
  for(i = 0; i < AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_FRAMES;){
    for(j = 0;
	j < AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_BUFFER_SIZE &&
	  i + j < AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_FRAMES;
	j++){
      CU_ASSERT(AGS_AUDIO_BUFFER_S16(stream->data)[j] == AGS_AUDIO_BUFFER_S16(template_stream->data)[j]);
    }

    /* iterate */
    stream = stream->next;
    template_stream = template_stream->next;
    
    i += AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_DEFAULTS_BUFFER_SIZE;
  }
}

void
ags_recycling_test_create_audio_signal_with_frame_count()
{
  AgsRecycling *recycling;
  AgsAudioSignal *template, *audio_signal;
  GList *stream, *template_stream;
  signed short *buffer;
  guint i, j;
  
  /* instantiate recycling */
  recycling = ags_recycling_new(G_OBJECT(devout));

  /* instantiate template audio signal */
  template = ags_audio_signal_new(G_OBJECT(devout),
				  recycling,
				  NULL);
  template->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
  g_object_set(G_OBJECT(template),
	       "samplerate\0", AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_SAMPLERATE,
	       "buffer-size\0", AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_BUFFER_SIZE,
	       "format\0", AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_FORMAT,
	       NULL);

  /* fill stream */
  stream = NULL;
  
  for(i = 0; i < AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_FRAMES;){
    buffer = (signed short *) malloc(template->buffer_size * sizeof(signed short));
    memset(buffer, 0, template->buffer_size * sizeof(signed short));
    
    for(j = 0;
	j < AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_BUFFER_SIZE &&
	  i + j < AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_FRAMES;
	j++){
      /* generate sin tone */
      buffer[j] = (signed short) (0xffff & (int) (32000.0 * (double) (sin ((double)(j) * 2.0 * M_PI * AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_FREQUENCY / (double) AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_SAMPLERATE))));
    }

    /* prepend buffer */
    stream = g_list_prepend(stream,
			    buffer);

    /* iterate */
    i += AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_BUFFER_SIZE;
  }

  template->stream_end = stream;
  template->stream_beginning = g_list_reverse(stream);
  
  /* add audio signal to recycling */
  ags_recycling_add_audio_signal(recycling,
				 template);

  /* instantiate audio signal */
  audio_signal = ags_audio_signal_new(G_OBJECT(devout),
				      recycling,
				      NULL);

  /* create frame count */
  ags_recycling_create_audio_signal_with_frame_count(recycling,
						     template,
						     0.0, 0,
						     AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_LAST_FRAME);

  /* assert audio signal */
  CU_ASSERT(audio_signal->samplerate == template->samplerate);
  CU_ASSERT(audio_signal->buffer_size == template->buffer_size);
  CU_ASSERT(audio_signal->format == template->format);

  CU_ASSERT(audio_signal->length == template->length);
  CU_ASSERT(audio_signal->first_frame == template->first_frame);
  CU_ASSERT(audio_signal->last_frame == AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_LAST_FRAME);
  CU_ASSERT(audio_signal->loop_start == template->loop_start);
  CU_ASSERT(audio_signal->loop_end == template->loop_end);

  stream = audio_signal->stream_beginning;
  template_stream = template->stream_beginning;
  
  for(i = 0; i < AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_LAST_FRAME;){
    for(j = 0;
	j < AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_BUFFER_SIZE &&
	  i + j < AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_LAST_FRAME;
	j++){
      CU_ASSERT(AGS_AUDIO_BUFFER_S16(stream->data)[j] == AGS_AUDIO_BUFFER_S16(template_stream->data)[j]);
    }

    /* iterate */
    stream = stream->next;
    template_stream = template_stream->next;
    
    i += AGS_RECYCLING_TEST_CREATE_AUDIO_SIGNAL_WITH_FRAME_COUNT_BUFFER_SIZE;
  }
}

void
ags_recycling_test_position()
{
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecycling *current;
  guint i;

  /* instantiate n recycling */
  first_recycling = NULL;
  last_recycling = NULL;
  
  for(i = 0; i < AGS_RECYCLING_TEST_POSITION_N_RECYCLING; i++){
    if(first_recycling == NULL){
      current = ags_recycling_new(G_OBJECT(devout));
      first_recycling = current;
    }else{
      current->next = ags_recycling_new(G_OBJECT(devout));
      current->next->prev = current;

      current = current->next;
    }
  }

  last_recycling = current;

  /* assert every position */
  current = first_recycling;
  
  for(i = 0; i < AGS_RECYCLING_TEST_POSITION_N_RECYCLING; i++){
    CU_ASSERT(ags_recycling_position(first_recycling, last_recycling,
				     current) == i);
    
    current = current->next;
  }
}

void
ags_recycling_test_find_next_channel()
{
  AgsChannel *start, *end;
  AgsChannel *channel;
  AgsRecycling *recycling;
  guint i;

  /* instantiate n channel and recycling */
  start = NULL;
  end = NULL;

  for(i = 0; i < AGS_RECYCLING_TEST_FIND_NEXT_CHANNEL_N_CHANNEL; i++){
    if(start == NULL){
      recycling = ags_recycling_new(G_OBJECT(devout));

      start = 
	channel = g_object_new(AGS_TYPE_CHANNEL,
			       "first-recycling\0", recycling,
			       NULL);

      g_object_set(G_OBJECT(recycling),
		   "channel\0", channel,
		   NULL);
    }else{
      recycling = ags_recycling_new(G_OBJECT(devout));
      
      channel->next = 
	channel->next_pad = g_object_new(AGS_TYPE_CHANNEL,
					 "first-recycling\0", recycling,
					 NULL);
      
      g_object_set(G_OBJECT(recycling),
		   "channel\0", channel,
		   NULL);
      
      channel->next->prev =
	channel->next_pad->prev_pad = channel;

      recycling->next = channel->next->first_recycling;
      channel->next->first_recycling->prev = recycling;
      
      channel = channel->next;
    }
  }

  end = channel;
  
  /* assert to find next channel */
  channel = start;
  recycling = start->first_recycling;
  
  for(i = 0; i < AGS_RECYCLING_TEST_FIND_NEXT_CHANNEL_N_CHANNEL; i++){
    CU_ASSERT(ags_recycling_find_next_channel(start->first_recycling, end->last_recycling,
					      channel) == recycling->next);

    channel = channel->next;
    recycling = recycling->next;
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
  pSuite = CU_add_suite("AgsRecyclingTest\0", ags_recycling_test_init_suite, ags_recycling_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of AgsRecycling add audio signal\0", ags_recycling_test_add_audio_signal) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecycling remove audio signal\0", ags_recycling_test_remove_audio_signal) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecycling create audio signal with defaults\0", ags_recycling_test_create_audio_signal_with_defaults) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecycling create audio signal with frame count\0", ags_recycling_test_create_audio_signal_with_frame_count) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecycling position\0", ags_recycling_test_position) == NULL) ||
     (CU_add_test(pSuite, "test of AgsRecycling find next channel\0", ags_recycling_test_find_next_channel) == NULL)){
      CU_cleanup_registry();
      
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

