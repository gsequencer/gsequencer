#include "ags_audio.h"

#include "../object/ags_marshal.h"

#include "ags_devout.h"
#include "ags_output.h"
#include "ags_input.h"
#include "ags_recall.h"
#include "ags_recall_id.h"

#include <stdlib.h>
#include <stdio.h>

GType ags_audio_get_type();
void ags_audio_class_init(AgsAudioClass *audio_class);
void ags_audio_init(AgsAudio *audio);
void ags_audio_finalize(GObject *gobject);

void ags_audio_real_set_audio_channels(AgsAudio *audio,
				       guint audio_channels, guint audio_channels_old);
void ags_audio_real_set_pads(AgsAudio *audio,
			     GType type,
			     guint channels, guint channels_old);

enum{
  SET_AUDIO_CHANNELS,
  SET_PADS,
  SET_LINES,
  LAST_SIGNAL,
};

static gpointer ags_audio_parent_class = NULL;
static guint audio_signals[LAST_SIGNAL];

GType
ags_audio_get_type (void)
{
  static GType ags_type_audio = 0;

  if(!ags_type_audio){
    static const GTypeInfo ags_audio_info = {
      sizeof(AgsAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_init,
    };

    ags_type_audio = g_type_register_static(G_TYPE_OBJECT,
					    "AgsAudio\0", &ags_audio_info,
					    0);
  }

  return(ags_type_audio);
}

void
ags_audio_class_init(AgsAudioClass *audio)
{
  GObjectClass *gobject;

  ags_audio_parent_class = g_type_class_peek_parent(audio);

  gobject = (GObjectClass *) audio;

  gobject->finalize = ags_audio_finalize;

  audio->set_audio_channels = ags_audio_real_set_audio_channels;
  audio->set_pads = ags_audio_real_set_pads;

  audio_signals[SET_AUDIO_CHANNELS] = 
    g_signal_new("set_audio_channels\0",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, set_audio_channels),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

  audio_signals[SET_PADS] = 
    g_signal_new("set_pads\0",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, set_pads),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__ULONG_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_ULONG,
		 G_TYPE_UINT, G_TYPE_UINT);
}

void
ags_audio_init(AgsAudio *audio)
{
  audio->flags = 0;

  audio->sequence_length = 0;
  audio->audio_channels = 0;
  audio->frequence = 0;

  audio->output_pads = 0;
  audio->output_lines = 0;
  audio->input_pads = 0;
  audio->input_lines = 0;

  audio->output = NULL;
  audio->input = NULL;

  audio->notation = NULL;

  audio->devout_play = ags_devout_play_alloc();
  audio->devout_play->source = (GObject *) audio;

  audio->recall_id = NULL;
  audio->recall = NULL;
  audio->play = 0;
  audio->recall_shared = NULL;

  audio->machine = NULL;
}

void
ags_audio_finalize(GObject *gobject)
{
  AgsAudio *audio;
  AgsChannel *channel;
  GList *list, *list_next;

  audio = AGS_AUDIO(gobject);

  /* recall */
  list = audio->recall;

  while(list != NULL){
    list_next = list->next;

    g_object_unref((GObject *) list->data);
    g_list_free1(list);

    list = list_next;
  }

  /* output */
  channel = audio->output;

  if(channel != NULL){
    while(channel->next != NULL){
      channel = channel->next;
      g_object_unref((GObject *) channel->prev);
    }

    g_object_unref((GObject *) channel);
  }

  /* input */
  channel = audio->input;

  if(channel != NULL){
    while(channel->next != NULL){
      channel = channel->next;
      g_object_unref((GObject *) channel->prev);
    }

    g_object_unref(channel);
  }

  /* notation */
  list = audio->notation;

  while(list != NULL){
    list_next = list->next;
   
    g_object_unref((GObject *) list->data);
    g_list_free1(list);

    list = list_next;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_audio_parent_class)->finalize(gobject);
}

void
ags_audio_connect(AgsAudio *audio)
{
}

/*
 * AgsInput has to be allocated first
 */
void
ags_audio_real_set_audio_channels(AgsAudio *audio,
				  guint audio_channels, guint audio_channels_old)
{
  AgsChannel *channel, *input, *input_pad_last;
  AgsRecycling *recycling;
  GObject *parent; // of recycling
  GList *list;
  guint pads, i, j;
  gboolean alloc_recycling;
  gboolean link_recycling; // affects AgsInput
  gboolean set_sync_link, set_async_link; // affects AgsOutput
  void ags_audio_set_audio_channels_init_parameters(GType type){
    if(type == AGS_TYPE_OUTPUT){
      link_recycling = FALSE;

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	alloc_recycling = TRUE;
      }else{
	alloc_recycling = FALSE;

	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  input = ags_channel_nth(audio->input, audio->audio_channels);

	  if((AGS_AUDIO_SYNC & (audio->flags)) != 0 && (AGS_AUDIO_ASYNC & (audio->flags)) == 0){
	    set_sync_link = FALSE;
	    set_async_link = TRUE;
	  }else if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	    set_async_link = TRUE;
	    set_sync_link = FALSE;
	    input_pad_last = ags_channel_nth(input, audio->input_lines - (audio_channels - audio->audio_channels));
	  }else{
	    fprintf(stdout, "ags_audio_set_audio_channels - warning: AGS_AUDIO_SYNC nor AGS_AUDIO_ASYNC weren't defined");
	    set_sync_link = FALSE;
	    set_async_link = FALSE;
	  }
	}
      }
    }else{
      set_sync_link = FALSE;
      set_async_link = FALSE;
      
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & audio->flags) != 0)
	alloc_recycling = TRUE;
      else
	alloc_recycling = FALSE;

      if((AGS_AUDIO_ASYNC & audio->flags) != 0 && alloc_recycling)
	link_recycling = TRUE;
      else
	link_recycling = FALSE;
    }    
  }
  void ags_audio_set_audio_channels_grow_one(GType type){
    channel = (AgsChannel *) g_object_new(type, NULL);

    if(type == AGS_TYPE_OUTPUT){
      /* AGS_TYPE_OUTPUT */
      audio->output = channel;
      pads = audio->output_pads;
    }else{
      /* AGS_TYPE_INPUT */
      audio->input = channel;
      pads = audio->input_pads;
    }

    channel->audio = (GObject *) audio;

    if(alloc_recycling){
      recycling =
	channel->first_recycling =
	channel->last_recycling = ags_recycling_new(audio->devout);

      channel->first_recycling->channel = (GObject *) channel;

      //      ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, recycling);
    }else if(set_sync_link){
      /* set sync link */
      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input->last_recycling;

      input = ags_channel_nth(input, audio_channels);
    }else if(set_async_link){

      /* set async link */
      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input_pad_last->last_recycling;

      input = input->next;
      input_pad_last = input_pad_last->next;
    }

    for(j = 1; j < pads; j++){
      channel->next =
	channel->next_pad = (AgsChannel *) g_object_new(type, NULL);
      channel->next->prev = channel;
      channel = channel->next;
      channel->prev_pad = channel->prev;

      channel->audio = (GObject *) audio;

      channel->pad = j;
      channel->line = j;

      if(alloc_recycling){
	channel->first_recycling =
	  channel->last_recycling = ags_recycling_new(audio->devout);

	if(link_recycling){
	  recycling->next = channel->first_recycling;
	  recycling->next->prev = recycling;
	  recycling = recycling->next;
	}

	channel->first_recycling->channel = (GObject *) channel;

	//	ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, channel->first_recycling);
      }else if(set_sync_link){
	/* set sync link */
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input->last_recycling;
	
	input = input->next;
      }
      /* set async link is illegal here */
    }
  }
  void ags_audio_set_audio_channels_grow(GType type){
    AgsChannel *prev_channel, *prev_pad, *next_channel, *start;
    AgsRecycling *prev_channel_next_recycling;

    if(type == AGS_TYPE_OUTPUT){
      //      prev_pad = audio->output;
      prev_channel = ags_channel_nth(audio->output, audio->audio_channels - 1);
      pads = audio->output_pads;
    }else{
      //      prev_pad = audio->input;
      prev_channel = ags_channel_nth(audio->input, audio->audio_channels - 1);
      pads = audio->input_pads;
    }

    /*
     * linking with prev_channel is done later else linked channels would be lost
     */
    start =
      channel = (AgsChannel *) g_object_new(type, NULL);

    channel->audio = (GObject *) audio;

    channel->audio_channel = audio->audio_channels;
    channel->line = audio->audio_channels;

    if(alloc_recycling){
      recycling =
	channel->first_recycling =
	channel->last_recycling = ags_recycling_new(audio->devout);

      if(link_recycling){
	prev_channel->last_recycling->next = recycling;
	recycling->prev = prev_channel->last_recycling;
      }

      channel->first_recycling->channel = (GObject *) channel;

      //      ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, recycling);
    }else if(set_sync_link){
      /* set sync link */
      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input->last_recycling;
      input = input->next;
    }else if(set_async_link){
      /* set async link*/
      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input_pad_last->last_recycling;

      input = input->next;
      input_pad_last = input_pad_last->next;
    }

    //    i = 1;

    /*
     * link now the new allocated channels
     */
    next_channel = prev_channel->next;

    prev_channel->next = start;
    start->prev = prev_channel;
    prev_channel = next_channel;

    for(j = audio->audio_channels + 1; j < audio_channels; j++){
      channel->next = (AgsChannel *) g_object_new(type, NULL);
      channel->next->prev = channel;
      channel = channel->next;

      channel->audio = (GObject *) audio;

      channel->pad = 0;
      channel->audio_channel = j;
      channel->line = j;

      if(alloc_recycling){
	channel->first_recycling =
	  channel->last_recycling = ags_recycling_new(audio->devout);

	if(link_recycling){
	  recycling->next = channel->first_recycling;
	  recycling->next->prev = recycling;
	  recycling = recycling->next;
	}

	channel->first_recycling->channel = (GObject *) channel;

	//	ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, channel->first_recycling);
      }else if(set_sync_link){
	/* set sync link */
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input->last_recycling;
	input = input->next;
      }else if(set_async_link){
	/* set async link */
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input_pad_last->last_recycling;

	input = input->next;
	input_pad_last = input_pad_last->next;
      }
    }

    /*
     * join together recycling
     */
    if(link_recycling){
      recycling->next = prev_channel->first_recycling;
      prev_channel->first_recycling->prev = recycling;
    }

    if(pads == 1)
      return;

    /*
     * the main part of allocation
     */
    for(i = 1; i < pads -1; i++){
      channel->next = prev_channel;
      prev_channel->prev = channel;

      /* set async link is illegal here */

      for(j = 0; j < audio->audio_channels -1; j++){
	prev_channel->line = i * audio_channels + j;

	prev_channel = prev_channel->next;

	if(link_recycling){
	  recycling = recycling->next;
	}
      }

      prev_channel->line = i * audio_channels + j;

      // prev_channel = prev_channel->next; will be called later

      if(link_recycling){
	recycling = recycling->next;
      }

      prev_pad = start;

      /* alloc new AgsChannel's */
      start =
	channel = (AgsChannel *) g_object_new(type, NULL);

      next_channel = prev_channel->next;

      prev_channel->next = start;
      start->prev = prev_channel;
      prev_channel = next_channel;

      j++;

      goto ags_audio_set_audio_channels_grow0;

      for(; j < audio_channels; j++){
	channel->next = (AgsChannel *) g_object_new(type, NULL);

	channel->next->prev = channel;
	channel = channel->next;

      ags_audio_set_audio_channels_grow0:

	prev_pad->next_pad = channel;
	channel->prev_pad = prev_pad;

	channel->audio = (GObject *) audio;

	channel->pad = i;
	channel->audio_channel = j;
	channel->line = (i * audio_channels) + j;

	if(alloc_recycling){
	  channel->first_recycling =
	    channel->last_recycling = ags_recycling_new(audio->devout);

	  if(link_recycling){
	    recycling->next = channel->first_recycling;
	    recycling->next->prev = recycling;
	    recycling = recycling->next;
	  }

	  channel->first_recycling->channel = (GObject *) channel;

	  //	  ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, channel->first_recycling);
	}else if(set_sync_link){
	  /* set sync link */
	  channel->first_recycling = input->first_recycling;
	  channel->last_recycling = input->last_recycling;
	  input = input->next;
	}
	/* set async link is illegal here */

	prev_pad = prev_pad->next;
      }

      if(link_recycling){
	recycling->next = prev_channel->first_recycling;
	prev_channel->first_recycling->prev = recycling;
      }
    }

    channel->next = prev_channel;
    prev_channel->prev = channel;

    /* set async link is illegal here */

    for(j = 0; j < audio->audio_channels -1; j++){
      prev_channel->line = i * audio_channels + j;

      prev_channel = prev_channel->next;

      if(link_recycling)
	recycling = recycling->next;
    }

    prev_channel->line = i * audio_channels + j;

    start =
      channel = (AgsChannel *) g_object_new(type, NULL);

    prev_channel->next = start;
    start->prev = prev_channel;
    j++;

    goto ags_audio_set_audio_channels_grow1;

    for(; j < audio_channels; j++){
      channel->next = (AgsChannel *) g_object_new(type, NULL);

      channel->next->prev = channel;
      channel = channel->next;

    ags_audio_set_audio_channels_grow1:

      prev_pad->next_pad = channel;
      channel->prev_pad = prev_pad;

      channel->audio = (GObject *) audio;

      channel->pad = i;
      channel->audio_channel = j;
      channel->line = (i * audio_channels) + j;

      if(alloc_recycling){
	channel->first_recycling =
	  channel->last_recycling = ags_recycling_new(audio->devout);

	if(link_recycling){
	  recycling->next = channel->first_recycling;
	  recycling->next->prev = recycling;
	  recycling = recycling->next;
	}

	channel->first_recycling->channel = (GObject *) channel;

	//	ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, channel->first_recycling);
      }else if(set_sync_link){
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input->last_recycling;
	input = input->next;
      }
      /* set async link is illegal here */

      prev_pad = prev_pad->next;
    }
  }
  void ags_audio_set_audio_channels_shrink_zero(){
    AgsChannel *start, *channel_next;
    AgsRecycling *prev_recycling, *next_recycling;
    gboolean first_run;

    start =
      channel = audio->output;
    first_run = TRUE;
  ags_audio_set_audio_channel_shrink_zero0:

    while(channel != NULL){
      ags_channel_set_link(channel, NULL);
      channel = channel->next;
    }

    channel = start;

    while(channel != NULL){
      channel_next = channel->next;

      g_object_unref((GObject *) channel);

      channel = channel_next;
    }

    if(first_run){
      start =
	channel = audio->input;
      first_run = FALSE;
      goto ags_audio_set_audio_channel_shrink_zero0;
    }

    audio->output = NULL;
    audio->input = NULL;
  }
  void ags_audio_set_audio_channels_shrink(){
    AgsChannel *channel0, *channel1, *start;
    AgsRecycling *recycling;
    gboolean first_run;

    start =
      channel = audio->output;
    pads = audio->output_pads;
    first_run = TRUE;

  ags_audio_set_audio_channel_shrink0:
    for(i = 0; i < pads; i++){
      channel = ags_channel_nth(channel, audio_channels);

      for(j = audio_channels; j < audio->audio_channels; j++){
	ags_channel_set_link(channel, NULL);
	channel = channel->next;
      }
    }

    channel = start;

    if(i < pads){
      for(i = 0; ; i++){
	for(j = 0; j < audio_channels -1; j++){
	  channel->pad = i;
	  channel->audio_channel = j;
	  channel->line = i * audio_channels + j;

	  channel = channel->next;
	}

	channel->pad = i;
	channel->audio_channel = j;
	channel->line = i * audio_channels + j;

	channel0 = channel->next;
	
	for(; j < audio->audio_channels; j++){
	  channel1 = channel0->next;

	  g_object_unref((GObject *) channel0);

	  channel0 = channel1;
	}

	channel->next = channel1;

	if(channel1 != NULL)
	  channel1->prev = channel;
	else
	  break;

	channel = channel1;
      }
    }

    if(first_run){
      first_run = FALSE;
      start =
	channel = audio->input;
      pads = audio->input_pads;

      goto ags_audio_set_audio_channel_shrink0;
    }
  }
  void ags_audio_set_audio_channels_grow_notation(){
    GList *list;
    guint i;

    i = audio->audio_channels;

    if(audio->audio_channels == 0){
      audio->notation =
	list = g_list_alloc();
      goto ags_audio_set_audio_channels_grow_notation0;
    }else{
      list = g_list_nth(audio->notation, audio->audio_channels - 1);
    }

    for(; i < audio_channels; i++){
      list->next = g_list_alloc();
      list->next->prev = list;
      list = list->next;

    ags_audio_set_audio_channels_grow_notation0:
      list->data = (gpointer) ags_notation_new();
    } 
  }
  void ags_audio_set_audio_channels_shrink_notation(){
    GList *list, *list_next;

    list = g_list_nth(audio->notation, audio_channels);

    if(audio_channels == 0){
      audio->notation = NULL;
    }else{
      list->prev->next = NULL;
    }

    while(list != NULL){
      list_next = list->next;

      g_object_unref((GObject *) list->data);
      g_list_free1(list);

      list = list_next;
    }
  }

  /* entry point */
  if(audio_channels > audio->audio_channels){
    /* grow audio channels*/
    if((AGS_AUDIO_HAS_NOTATION & audio->flags) != 0)
      ags_audio_set_audio_channels_grow_notation();

    if(audio->audio_channels == 0){
      if(audio->input_pads > 0){
	ags_audio_set_audio_channels_init_parameters(AGS_TYPE_INPUT);
	ags_audio_set_audio_channels_grow_one(AGS_TYPE_INPUT);
      }

      audio->input_lines = audio->input_pads;

      if(audio->output_pads > 0){
	ags_audio_set_audio_channels_init_parameters(AGS_TYPE_OUTPUT);
	ags_audio_set_audio_channels_grow_one(AGS_TYPE_OUTPUT);
      }

      audio->audio_channels = 1;
      audio->output_lines = audio->output_pads;
    }

    if(audio_channels > 1){
      if(audio->input_pads > 0){
	ags_audio_set_audio_channels_init_parameters(AGS_TYPE_INPUT);
	ags_audio_set_audio_channels_grow(AGS_TYPE_INPUT);
      }

      audio->input_lines = audio_channels * audio->input_pads;

      if(audio->output_pads > 0){
	ags_audio_set_audio_channels_init_parameters(AGS_TYPE_OUTPUT);
     	ags_audio_set_audio_channels_grow(AGS_TYPE_OUTPUT);
      }
    }
  }else if(audio_channels < audio->audio_channels){
    /* shrink audio channels */
    if((AGS_AUDIO_HAS_NOTATION & audio->flags) != 0)
      ags_audio_set_audio_channels_shrink_notation();

    if(audio_channels == 0){
      ags_audio_set_audio_channels_shrink_zero();
    }else{
      ags_audio_set_audio_channels_shrink();
    }

    audio->input_lines = audio_channels * audio->input_pads;
  }

  audio->audio_channels = audio_channels;
  // input_lines must be set earlier because set_sync_link needs it
  audio->output_lines = audio_channels * audio->output_pads;
}

void
ags_audio_set_audio_channels(AgsAudio *audio, guint audio_channels)
{
  g_return_if_fail(AGS_IS_AUDIO(audio));

  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[SET_AUDIO_CHANNELS], 0,
		audio_channels, audio->audio_channels);
  g_object_unref((GObject *) audio);
}

/*
 * AgsInput has to be allocated first
 */
void
ags_audio_real_set_pads(AgsAudio *audio,
			GType type,
			guint pads, guint pads_old)
{
  AgsChannel *start, *channel, *prev_pad, *input, *input_pad_last;
  AgsRecycling *recycling;
  guint i, j;
  gboolean alloc_recycling, link_recycling, set_sync_link, update_async_link, set_async_link;
  void ags_audio_set_pads_grow_one(){
    start =
      channel = (AgsChannel *) g_object_new(type, NULL);

    channel->audio = (GObject *) audio;

    if(alloc_recycling){
      channel->first_recycling =
	channel->last_recycling = ags_recycling_new(audio->devout);

      if(link_recycling)
	recycling = channel->first_recycling;

      channel->first_recycling->channel = (GObject *) channel;

      //      ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, channel->first_recycling);
    }else if(set_sync_link){
      input = audio->input;
      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input->last_recycling;
      input = input->next;
    }else if(set_async_link){
      input = audio->input;
      input_pad_last = ags_channel_nth(audio->input, audio->input_lines - audio->audio_channels);

      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input_pad_last->last_recycling;

      input = input->next;
      input_pad_last = input_pad_last->next;
    }

    for(j = 1; j < audio->audio_channels; j++){
      channel->next = (AgsChannel *) g_object_new(type, NULL);
      channel->next->prev = channel;
      channel = channel->next;

      channel->audio = (GObject *) audio;

      channel->audio_channel = j;
      channel->line = j;

      if(alloc_recycling){
	channel->first_recycling =
	  channel->last_recycling = ags_recycling_new(audio->devout);

	if(link_recycling){
	  recycling->next = channel->first_recycling;
	  recycling->next->prev = recycling;
	  recycling = recycling->next;
	}

	channel->first_recycling->channel = (GObject *) channel;

	//	ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, channel->first_recycling);
      }else if(set_sync_link){
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input->last_recycling;

	input = input->next;
      }else if(set_async_link){
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input_pad_last->last_recycling;

	input = input->next;
	input_pad_last = input_pad_last->next;
      }
    }
  }
  void ags_audio_set_pads_grow(){
    AgsRecycling *recycling_next;

    prev_pad = ags_channel_nth(channel, (pads_old - 1) * audio->audio_channels);
    channel = ags_channel_last(prev_pad);

    if(alloc_recycling){
      recycling = channel->last_recycling;
      recycling_next = channel->last_recycling->next;
    }else if(set_sync_link){
      if(pads_old != 0)
	input = ags_channel_nth(audio->input, pads_old);
    }

    for(i = pads_old; i < pads; i++){
      if(set_async_link){
	input = audio->input;
	input_pad_last = ags_channel_nth(audio->input, audio->input_lines - audio->audio_channels);
      }

      for(j = 0; j < audio->audio_channels; j++){
	channel->next = (AgsChannel *) g_object_new(type, NULL);
	channel->next->prev = channel;
	channel = channel->next;

	channel->prev_pad = prev_pad;
	prev_pad->next_pad = channel;

	channel->audio = (GObject *) audio;

	channel->pad = i;
	channel->audio_channel = j;
	channel->line = i * audio->audio_channels + j;

	if(alloc_recycling){
	  channel->first_recycling =
	    channel->last_recycling = ags_recycling_new(audio->devout);

	  if(link_recycling){
	    recycling->next = channel->first_recycling;
	    recycling->next->prev = recycling;
	    recycling = recycling->next;
	  }

	  channel->first_recycling->channel = (GObject *) channel;

	  //	  ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, channel->first_recycling);
	}else if(set_sync_link){
	  channel->first_recycling = input->first_recycling;
	  channel->last_recycling = input->last_recycling;

	  input = input->next;
	}else if(set_async_link){
	  channel->first_recycling = input->first_recycling;
	  channel->last_recycling = input_pad_last->last_recycling;

	  input = input->next;
	  input_pad_last = input_pad_last->next;
	}

	prev_pad = prev_pad->next;
      }
    }

    if(alloc_recycling && link_recycling){
      recycling->next = recycling_next;

      if(recycling_next != NULL)
	recycling_next->prev = recycling;
    }
  }
  void ags_audio_set_pads_shrink_zero(){
    AgsChannel *start, *channel_next;

    start = channel;

    while(channel != NULL){
      ags_channel_set_link(channel, NULL);

      channel = channel->next;
    }

    channel = start;

    while(channel != NULL){
      channel_next = channel->next;

      g_object_unref((GObject *) channel);

      channel = channel_next;
    }
  }
  void ags_audio_set_pads_shrink(){
    channel = ags_channel_nth(channel, (pads - 1) * audio->audio_channels);

    for(i = 0; i < audio->audio_channels; i++){
      channel->next_pad = NULL;

      channel = channel->next;
    }

    channel->prev->next = NULL;
    ags_audio_set_pads_shrink_zero();
  }
  void ags_audio_set_pads_alloc_notation(){
    GList *list;
    guint i;

    fprintf(stdout, "ags_audio_set_pads_alloc_notation\n\0");

    if(audio->audio_channels > 0){
      audio->notation =
	list = g_list_alloc();
      i = 0;
      goto ags_audio_set_pads_alloc_notation0;
    }else
      return;

    for(; i < audio->audio_channels; i++){
      list->next = g_list_alloc();
      list->next->prev = list;
      list = list->next;
    ags_audio_set_pads_alloc_notation0:

      list->data = (gpointer) ags_notation_new();
    }
  }
  void ags_audio_set_pads_free_notation(){
    GList *list, *list_next;

    if(audio->audio_channels > 0){
      list = audio->notation;
    }else
      return;

    while(list != NULL){
      list_next = list->next;

      g_object_unref(G_OBJECT(list->data));
      g_list_free1(list);

      list = list_next;
    }
  }
  void ags_audio_set_pads_add_notes(){
    GList *list;

    list = audio->notation;

    while(list != NULL){
      AGS_NOTATION(list->data)->pads = pads;

      list = list->next;
    }
  }
  void ags_audio_set_pads_remove_notes(){
    AgsNotation *notation;
    GList *notation_i, *note, *note_next;

    notation_i = audio->notation;

    while(notation != NULL){
      notation = AGS_NOTATION(notation_i->data);
      note = notation->note;

      while(note != NULL){
	note_next = note->next;

	if(AGS_NOTE(note->data)->y >= pads){
	  if(note->prev != NULL)
	    note->prev->next = note_next;
	  else
	    notation->note = note_next;

	  if(note_next != NULL)
	    note_next->prev = note->prev;

	  free(note->data);
	  g_list_free1(note);
	}

	note = note_next;
      }

      notation_i = notation_i->next;
    }
  }

  alloc_recycling = FALSE;
  link_recycling = FALSE;
  set_sync_link = FALSE;
  update_async_link = FALSE;
  set_async_link = FALSE;

  if(type == AGS_TYPE_OUTPUT){
    pads_old = audio->output_pads;
    link_recycling = FALSE;

    if(pads_old == pads)
      return;

    if(audio->audio_channels == 0){
      audio->output_pads = pads;
      return;
    }

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
      alloc_recycling = TRUE;
    else if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      if((AGS_AUDIO_SYNC & audio->flags) != 0 && (AGS_AUDIO_ASYNC & audio->flags) == 0)
	set_sync_link = TRUE;
      else if((AGS_AUDIO_ASYNC & audio->flags) == 0)
	input = audio->input;

    if(pads_old == 0){
      if((AGS_AUDIO_HAS_NOTATION & (audio->flags)) != 0 &&
	 audio->notation == NULL){
	ags_audio_set_pads_alloc_notation();
	ags_audio_set_pads_add_notes();
      }

      ags_audio_set_pads_grow_one();
      channel =
	audio->output = start;
      pads_old =
	audio->output_pads = 1;
    }else
      channel = audio->output;

    if(pads > audio->output_pads){
      ags_audio_set_pads_grow();
    }else if(pads == 0){
      if((AGS_AUDIO_HAS_NOTATION & (audio->flags)) != 0 &&
	 audio->notation != NULL)
	ags_audio_set_pads_free_notation();

      ags_audio_set_pads_shrink_zero();
      audio->output = NULL;
    }else if(pads < audio->output_pads){
      ags_audio_set_pads_remove_notes();
      ags_audio_set_pads_shrink();
    }

    audio->output_pads = pads;
    audio->output_lines = pads * audio->audio_channels;
  }else{
    pads_old = audio->input_pads;

    if(pads_old == pads)
      return;

    if(audio->audio_channels == 0){
      audio->input_pads = pads;
      return;
    }

    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
      alloc_recycling = TRUE;

      if((AGS_AUDIO_ASYNC & audio->flags) != 0){
	link_recycling = TRUE;

	if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0)
	  update_async_link = TRUE;
      }
    }
    
    if(pads_old == 0){
      ags_audio_set_pads_grow_one();
      channel =
	audio->input = start;
      pads_old =
	audio->input_pads = 1;
    }else
      channel = audio->input;

    if(pads > 1)
      if(pads > audio->input_pads)
	ags_audio_set_pads_grow();
      else if(pads == 0){
	ags_audio_set_pads_shrink_zero();
	audio->input = NULL;
      }else if(pads < audio->input_pads)
	ags_audio_set_pads_shrink();

    if(update_async_link){
      channel = audio->output;

      input = audio->input;
      input_pad_last = ags_channel_nth(audio->input, (audio->input_lines - 1) * audio->audio_channels);

      for(j = 0; j < audio->audio_channels; j++){
	ags_channel_set_recycling(channel,
				  input->first_recycling, input_pad_last->last_recycling,
				  TRUE, FALSE);
	channel = channel->next;
	input = input->next;
	input_pad_last = input_pad_last->next;
      }
    }

    audio->input_pads = pads;
    audio->input_lines = pads * audio->audio_channels;
  }
}

void
ags_audio_set_pads(AgsAudio *audio, GType type, guint pads)
{
  guint pads_old;

  g_return_if_fail(AGS_IS_AUDIO(audio));

  g_object_ref((GObject *) audio);
  pads_old = (type == AGS_TYPE_OUTPUT) ? audio->output_pads: audio->input_pads;
  g_signal_emit(G_OBJECT(audio),
		audio_signals[SET_PADS], 0,
		type, pads, pads_old);
  g_object_unref((GObject *) audio);
}

void
ags_audio_set_devout(AgsAudio *audio, GObject *devout)
{
  void ags_audio_set_devout_real(AgsChannel *channel){
    AgsRecycling *recycling;
    AgsAudioSignal *audio_signal;

    while(channel != NULL){
      recycling = channel->first_recycling;

      while(recycling != channel->last_recycling->next){
	audio_signal = ags_audio_signal_get_template(recycling->audio_signal);
	audio_signal->devout = devout;

	recycling = recycling->next;
      }

      channel = channel->next;
    }
  }

  audio->devout = devout;

  if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
    ags_audio_set_devout_real(audio->input);

  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
    ags_audio_set_devout_real(audio->output);
}

void
ags_audio_set_sequence_length(AgsAudio *audio, guint sequence_length)
{
  audio->sequence_length = sequence_length;
}

void
ags_audio_map_delay(AgsAudio *audio)
{
}

void
ags_audio_play(AgsAudio *audio, guint group_id, gint stage, gboolean do_recall)
{
  AgsRecall *recall;
  AgsRecallID *recall_id;
  GList *list, *list_next;
  
  if(do_recall)
    list = audio->recall;
  else
    list = audio->play;
  
  recall_id = ags_recall_id_find_group_id(audio->recall_id, group_id);

  while(list != NULL){
    list_next = list->next;

    recall = AGS_RECALL(list->data);
    
    if((AGS_RECALL_HIDE & recall->flags) == 0){
      if(stage == 0)
	ags_recall_run_pre(recall);
      else if(stage == 1)
	ags_recall_run_inter(recall);
      else
	ags_recall_run_post(recall);
    }

    ags_recall_check_cancel(recall);

    ags_recall_child_check_remove(recall);

    if((AGS_RECALL_REMOVE & (recall->flags)) != 0){
      if(do_recall)
	audio->recall = g_list_remove(audio->recall, recall);
      else
	audio->play = g_list_remove(audio->play, recall);

      ags_recall_remove(recall);
    }
    
    list = list_next;
  }
}

guint
ags_audio_recursive_play_init(AgsAudio *audio)
{
  AgsChannel *channel;
  guint group_id, child_group_id;
  gint stage;

  group_id = ags_recall_id_generate_group_id();
  child_group_id = ags_recall_id_generate_group_id();

  for(stage = 0; stage < 3; stage++){
    channel = audio->output;

    while(channel != NULL){
      ags_channel_recursive_play_init(channel, stage,
				      (stage == 0 ? TRUE: FALSE),
				      group_id, child_group_id);

      channel = channel->next;
    }
  }

  return(group_id);
}

AgsAudio*
ags_audio_new()
{
  AgsAudio *audio;

  audio = (AgsAudio *) g_object_new(AGS_TYPE_AUDIO, NULL);

  return(audio);
}
