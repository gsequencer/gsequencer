#include "ags_channel.h"

#include "../object/ags_connectable.h"
#include "../object/ags_run_connectable.h"
#include "../object/ags_marshal.h"

#include "ags_audio.h"
#include "ags_output.h"
#include "ags_input.h"
#include "ags_audio_signal.h"
#include "ags_pattern.h"
#include "ags_recall.h"
#include "ags_recall_audio.h"
#include "ags_recall_audio_run.h"
#include "ags_recall_channel.h"
#include "ags_recall_channel_run.h"

#include <stdio.h>

GType ags_channel_get_type();
void ags_channel_class_init(AgsChannelClass *channel_class);
void ags_channel_init(AgsChannel *channel);
void ags_channel_finalize(GObject *gobject);

extern void ags_file_write_channel(AgsFile *file, AgsChannel *channel);

enum{
  RECYCLING_CHANGED,
  LAST_SIGNAL,
};

static gpointer ags_channel_parent_class = NULL;
static guint channel_signals[LAST_SIGNAL];

GType
ags_channel_get_type (void)
{
  static GType ags_type_channel = 0;

  if(!ags_type_channel){
    static const GTypeInfo ags_channel_info = {
      sizeof (AgsChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_channel_init,
    };

    ags_type_channel = g_type_register_static(G_TYPE_OBJECT,
					      "AgsChannel\0",
					      &ags_channel_info, 0);
  }

  return(ags_type_channel);
}

void
ags_channel_class_init(AgsChannelClass *channel)
{
  GObjectClass *gobject;

  ags_channel_parent_class = g_type_class_peek_parent(channel);

  gobject = (GObjectClass *) channel;
  gobject->finalize = ags_channel_finalize;

  channel->write_file = ags_file_write_channel;

  channel->recycling_changed = NULL;

  channel_signals[RECYCLING_CHANGED] =
    g_signal_new("recycling_changed\0",
		 G_TYPE_FROM_CLASS (channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsChannelClass, recycling_changed),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__OBJECT_OBJECT_OBJECT_OBJECT,
		 G_TYPE_NONE, 4,
		 G_TYPE_OBJECT, G_TYPE_OBJECT,
		 G_TYPE_OBJECT, G_TYPE_OBJECT);
}

void
ags_channel_init(AgsChannel *channel)
{
  channel->flags = 0;

  channel->audio = NULL;

  channel->prev = NULL;
  channel->prev_pad = NULL;
  channel->next = NULL;
  channel->next_pad = NULL;

  channel->pad = 0;
  channel->audio_channel = 0;
  channel->line = 0;

  channel->note = NULL;

  channel->devout_play = ags_devout_play_alloc();
  channel->devout_play->source = (GObject *) channel;

  channel->recall_id = NULL;
  channel->recall = NULL;
  channel->play = NULL;

  channel->link = NULL;
  channel->first_recycling = NULL;
  channel->last_recycling = NULL;

  channel->pattern = NULL;
  channel->notation = NULL;

  channel->line_widget = NULL;
  channel->file_data = NULL;
}

void
ags_channel_finalize(GObject *gobject)
{
  AgsChannel *channel;
  AgsRecycling *recycling, *recycling_next;
  GList *list, *list_next;

  channel = AGS_CHANNEL(gobject);

  if(((AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0 && AGS_IS_INPUT(channel)) ||
     ((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0 && AGS_IS_OUTPUT(channel))){

    recycling = channel->first_recycling;
    //    ags_garbage_collector_remove(AGS_DEVOUT(AGS_AUDIO(channel->audio)->devout)->garbage_collector, recycling);

    if(recycling != NULL)
      while(recycling != channel->last_recycling->next){
	recycling_next = recycling->next;

	//	ags_garbage_collector_remove(AGS_DEVOUT(AGS_AUDIO(channel->audio)->devout)->garbage_collector, recycling);
	g_object_unref((GObject *) recycling);

	recycling = recycling_next;
      }
  }

  /* AgsRecallID */
  list = channel->recall_id;

  while(list != NULL){
    list_next = list->next;

    g_object_unref(G_OBJECT(list->data));
    g_list_free1(list);

    list = list_next;
  }
  
  /* AgsRecall - recall */
  list = channel->recall;

  while(list != NULL){
    list_next = list->next;

    g_object_unref(G_OBJECT(list->data));
    g_list_free1(list);

    list = list_next;
  }

  /* AgsRecall - play */
  list = channel->play;

  while(list != NULL){
    list_next = list->next;

    g_object_unref(G_OBJECT(list->data));
    g_list_free1(list);

    list = list_next;
  }

  /* AgsPattern */
  list = channel->pattern;

  while(list != NULL){
    list_next = list->next;

    g_object_unref(G_OBJECT(list->data));
    g_list_free1(list);

    list = list_next;
  }

  G_OBJECT_CLASS(ags_channel_parent_class)->finalize(gobject);
}

void
ags_channel_connect(AgsChannel *channel)
{
}

AgsRecall*
ags_channel_find_recall(AgsChannel *channel, char *effect, char *name)
{
  /* */
}

AgsChannel*
ags_channel_last(AgsChannel *channel)
{
  while(channel->next_pad != NULL)
    channel = channel->next_pad;

  while(channel->next != NULL)
    channel = channel->next;

  return(channel);
}

AgsChannel*
ags_channel_nth(AgsChannel *channel, guint nth)
{
  guint i;

  for(i = 0; i < nth && channel != NULL; i++){
    channel = channel->next;
  }

  if((nth != 0 && i != nth) || channel == NULL)
    fprintf(stderr, "ags_channel_nth:\n  nth channel does not exist\n  `- stopped @: i = %u; nth = %u\n\0", i, nth);

  return(channel);
}

AgsChannel*
ags_channel_nth_pad(AgsChannel *channel, guint nth)
{
  guint i;

  for(i = 0; i < nth && channel != NULL; i++)
    channel = channel->next_pad;

  if((nth != 0 && i != nth) || channel == NULL)
    fprintf(stderr, "ags_channel_nth_pad:\n  nth pad does not exist\n  `- stopped @: i = %u; nth = %u\0", i, nth);

  return(channel);
}

void
ags_channel_set_link(AgsChannel *channel, AgsChannel *link)
{
  AgsChannel *output, *output_link;
  AgsChannel *input, *input_link;
  AgsRecycling *recycling;

  if(channel == NULL){
    if(link == NULL)
      return;

    channel = link;
    link = NULL;
  }

  if(AGS_IS_INPUT(channel)){
    output = link;
    input = channel;
  }else{
    output = channel;
    input = link;
  }

  /* AgsInput */
  if(input != NULL){
    input_link = input->link;
    input->link = output;
  }else
    input_link = NULL;

  if(input_link != NULL){
    input_link->link = NULL;
  }

  /* AgsOutput */
  if(output != NULL){
    output_link = output->link;
    output->link = input;
  }else
    output_link = NULL;

  if(output_link != NULL){
    output_link->link = NULL;
  }

  /* reset recycling */
  if(input_link != NULL){
    gboolean set_parent_to_null;

    recycling = input_link->first_recycling;
    set_parent_to_null = ((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (AGS_AUDIO(input_link->audio)->flags)) != 0) ? TRUE: FALSE;

    while(recycling != input_link->last_recycling->next){
      if(set_parent_to_null)
	recycling->parent = NULL;
      else
	recycling->parent = (GObject *) output;

      recycling = recycling->next;
    }
  }

  if(output_link != NULL){
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(output_link->audio)->flags)) != 0){
      recycling = ags_recycling_new(AGS_AUDIO(output_link->audio)->devout);
      recycling->channel = (GObject *) output_link;

      ags_channel_set_recycling(output_link,
				recycling, recycling,
				TRUE, FALSE);
    }else{
      ags_channel_set_recycling(output_link,
	 			NULL, NULL,
				TRUE, FALSE);
    }
  }

  if(input != NULL){
    if(output != NULL){
      if(input_link == NULL && (AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(input->audio)->flags)) != 0)
	g_object_unref(G_OBJECT(input->first_recycling));

      ags_channel_set_recycling(input,
				output->first_recycling, output->last_recycling,
				TRUE, FALSE);
    }else{
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(input->audio)->flags)) != 0){
	recycling = ags_recycling_new(AGS_AUDIO(input->audio)->devout);
	recycling->channel = (GObject *) input;

	ags_channel_set_recycling(input,
				  recycling, recycling,
				  TRUE, FALSE);
      }else{
	ags_channel_set_recycling(input,
				  NULL, NULL,
				  TRUE, FALSE);
      }
    }
  }else{
    gboolean set_parent_to_null;

    recycling = output->first_recycling;
    set_parent_to_null = ((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (AGS_AUDIO(output->audio)->flags)) != 0) ? TRUE: FALSE;

    while(recycling != output->last_recycling->next){
      if(set_parent_to_null)
	recycling->parent = NULL;
      else
	recycling->parent = (GObject *) output;

      recycling = recycling->next;
    }
  }
}

/*
 * channel - the channel to reset
 * first_recycling - the recycling to set for channel->first_recycling
 * last_recycling - the recycling to set for channel->last_recycling
 * update - reset even the AgsRecyclings are still the same
 * destroy_old - destroys old AgsRecyclings
 */
void
ags_channel_set_recycling(AgsChannel *channel,
			  AgsRecycling *first_recycling, AgsRecycling *last_recycling,
			  gboolean update, gboolean destroy_old)
{
  AgsAudio *audio;
  AgsRecycling *old_first_recycling, *old_last_recycling;
  AgsRecycling *nth_recycling, *next_recycling, *stop_recycling;
  GObject *parent;
  auto void ags_channel_set_first_recycling(AgsChannel *input);
  auto void ags_channel_set_last_recycling(AgsChannel *input);
  auto gboolean ags_channel_set_recycling_emit_changed(AgsChannel *input);
  gboolean ags_channel_set_first_recycling_input(AgsChannel *input){
    AgsChannel *nth_channel;

    input->first_recycling = first_recycling;
    
    if((AGS_AUDIO_ASYNC & (AGS_AUDIO(input->audio)->flags)) != 0){
      nth_channel = input->prev_pad;
    
      while(nth_channel != NULL && nth_channel->last_recycling == NULL){
	nth_channel = nth_channel->prev_pad;
      }
    
      if(nth_channel != NULL){
	if(input->first_recycling != NULL){
	  input->first_recycling->prev = nth_channel->last_recycling;
	}
      
	nth_channel->last_recycling->next = input->first_recycling;
	return(TRUE);
      }
    }

    return(FALSE);
  }
  void ags_channel_set_first_recycling_output(AgsChannel *output){
    output->first_recycling = first_recycling;
	  
    if(output->link != NULL)
      ags_channel_set_first_recycling(output->link);
  }
  void ags_channel_set_first_recycling(AgsChannel *input){
    AgsAudio *audio;
    AgsChannel *output;

    audio = AGS_AUDIO(input->audio);

    /* AgsInput */
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
      if(ags_channel_set_first_recycling_input(input))
	return;
    }else
      return;

    /* AgsOutput */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0)
	output = ags_channel_nth(audio->output, input->audio_channel);
      else
	output = ags_channel_nth(audio->output, input->line);

      ags_channel_set_first_recycling_output(output);
    }
  }
  gboolean ags_channel_set_last_recycling_input(AgsChannel *input){
    AgsChannel *nth_channel;

    input->last_recycling = last_recycling;

    if((AGS_AUDIO_ASYNC & AGS_AUDIO(input->audio)->flags) != 0){
      nth_channel = input->next_pad;

      while(nth_channel != NULL && nth_channel->first_recycling == NULL){
	nth_channel = nth_channel->next_pad;
      }

      if(nth_channel != NULL){
	if(input->last_recycling != NULL){
	  input->last_recycling->next = nth_channel->first_recycling;
	}

	nth_channel->first_recycling->prev = input->last_recycling;

	return(TRUE);
      }
    }

    return(FALSE);
  }
  void ags_channel_set_last_recycling_output(AgsChannel *output){
    output->last_recycling = last_recycling;

    if(output->link != NULL){
      ags_channel_set_last_recycling(output->link);
    }
  }
  void ags_channel_set_last_recycling(AgsChannel *input){
    AgsAudio *audio;
    AgsChannel *output;

    audio = AGS_AUDIO(input->audio);

    /* AgsInput */
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
      if(ags_channel_set_last_recycling_input(input))
	return;
    }else
      return;

    /* AgsOutput */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0)
	output = ags_channel_nth(audio->output, input->audio_channel);
      else
	output = ags_channel_nth(audio->output, input->line);

      ags_channel_set_last_recycling_output(output);
    }
  }
  void ags_channel_set_recycling_emit_changed_input(AgsChannel *input){
    ags_channel_recycling_changed(input,
				  old_first_recycling, old_last_recycling,
				  first_recycling, last_recycling);
  }
  void ags_channel_set_recycling_emit_changed_output(AgsChannel *output){
    ags_channel_recycling_changed(output,
				  old_first_recycling, old_last_recycling,
				  first_recycling, last_recycling);

    if(output->link != NULL)
      if(ags_channel_set_recycling_emit_changed(output->link))
	parent = (GObject *) output;
    else
      parent = (GObject *) output;
  }
  gboolean ags_channel_set_recycling_emit_changed(AgsChannel *input){
    AgsAudio *audio;
    AgsChannel *output;

    audio = AGS_AUDIO(input->audio);

    /* AgsInput */
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0)
      ags_channel_set_recycling_emit_changed_input(input);
    else
      return(TRUE);

    output = ags_channel_nth(audio->output, (((AGS_AUDIO_ASYNC & (audio->flags)) == 0) ? input->audio_channel: input->line));
      
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0)
      ags_channel_set_recycling_emit_changed_output(output);
    else
      parent = (GObject *) output;

    return(FALSE);
  }

  /* entry point */
  audio = AGS_AUDIO(channel->audio);

  if(first_recycling == NULL && last_recycling != NULL)
    first_recycling = last_recycling;

  if(last_recycling == NULL && first_recycling != NULL)
    last_recycling = first_recycling;

  old_first_recycling = channel->first_recycling;
  old_last_recycling = channel->last_recycling;

  if(!update && old_first_recycling == first_recycling && old_last_recycling == last_recycling)
    return;

  parent = NULL;

  if(AGS_IS_INPUT(channel)){
    AgsChannel *output;

    /* first_recycling */
    if(update || old_first_recycling != first_recycling){
      /* AgsInput */
      ags_channel_set_first_recycling_input(channel);

      /* AgsOutput */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
	if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	  output = ags_channel_nth(audio->output, channel->audio_channel);
	}else{
	  output = ags_channel_nth(audio->output, channel->line);
	}

	ags_channel_set_first_recycling_output(output);
      }else if(first_recycling != NULL){
	guint nth;

	nth = ((AGS_AUDIO_ASYNC & (audio->flags)) != 0) ? channel->audio_channel: channel->line;
	parent = (GObject *) ags_channel_nth(audio->output, nth);
      }
    }

    /* last_recycling */
    if(update || old_last_recycling != last_recycling){
      /* AgsInput */
      ags_channel_set_last_recycling_input(channel);
      
      /* AgsOutput */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
	if((AGS_AUDIO_ASYNC & (audio->flags)) != 0)
	  output = ags_channel_nth(audio->output, channel->audio_channel);
	else
	  output = ags_channel_nth(audio->output, channel->line);

      	ags_channel_set_last_recycling_output(output);
      }
    }
  }else{
    /* AgsOutput */
    channel->first_recycling = first_recycling;
    channel->last_recycling = last_recycling;

    /* AgsInput */
    if(channel->link != NULL){
      if(update || old_first_recycling != first_recycling)
	ags_channel_set_first_recycling(channel->link);

      if(update || old_last_recycling != last_recycling)
	ags_channel_set_last_recycling(channel->link);
    }
  }

  /* destroy old recycling */
  if(destroy_old && old_first_recycling != NULL){
    if(old_first_recycling == first_recycling){
      nth_recycling = (last_recycling != NULL) ? last_recycling->next: NULL;
      stop_recycling = old_last_recycling->next;
    }else{
      nth_recycling = old_first_recycling;
      stop_recycling = first_recycling;
    }

    while(nth_recycling != stop_recycling){
      next_recycling = nth_recycling->next;
      
      g_object_unref(G_OBJECT(nth_recycling));
      
      nth_recycling = next_recycling;
    }
  }
  
  /* set parent */
  if(AGS_IS_INPUT(channel)){
    AgsChannel *output;

    ags_channel_set_recycling_emit_changed_input(channel);

    output = ags_channel_nth(audio->output, (((AGS_AUDIO_ASYNC & (audio->flags)) == 0) ? channel->audio_channel: channel->line));
    
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0)
      ags_channel_set_recycling_emit_changed_output(output);
    else
      parent = (GObject *) output;
  }else{
    ags_channel_set_recycling_emit_changed_output(channel);
  }

  if(first_recycling != NULL){
    if(update){
      nth_recycling = first_recycling;
      stop_recycling = last_recycling->next;
    }else{
      if(first_recycling == old_first_recycling){
	nth_recycling = old_last_recycling;
	stop_recycling = last_recycling->next;
      }else{
	nth_recycling = first_recycling;
	stop_recycling = (old_first_recycling != NULL) ? old_first_recycling->next: NULL;
      }
    }

    while(nth_recycling != stop_recycling){
      nth_recycling->parent = parent;

      nth_recycling = nth_recycling->next;
    }
  }
}

/*
 * channel - the affected channel
 */
void
ags_channel_recycling_changed(AgsChannel *channel,
			      AgsRecycling *old_start_region, AgsRecycling *old_end_region,
			      AgsRecycling *new_start_region, AgsRecycling *new_end_region)
{
  g_return_if_fail(AGS_IS_CHANNEL(channel));

  g_object_ref(G_OBJECT(channel));
  g_signal_emit(G_OBJECT(channel),
		channel_signals[RECYCLING_CHANGED], 0,
		old_start_region, old_end_region,
		new_start_region, new_end_region);
  g_object_unref(G_OBJECT(channel));
}

void
ags_channel_resize_audio_signal(AgsChannel *channel, guint size)
{
  AgsAudioSignal *audio_signal;
  GList *list_recall_id;
  GList *list_audio_signal;
  guint size_old;

  audio_signal = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
  size_old = audio_signal->length;
  ags_audio_signal_stream_resize(audio_signal, size);

  list_recall_id = channel->recall_id;

  while(list_recall_id != NULL){
    list_audio_signal = channel->first_recycling->audio_signal;

    while((list_audio_signal = ags_audio_signal_get_by_recall_id(list_audio_signal,
								 (GObject *) list_recall_id->data))
	  != NULL){
      audio_signal = AGS_AUDIO_SIGNAL(list_audio_signal->data);

      if(audio_signal != NULL){
	if(size_old < size)
	  ags_audio_signal_stream_resize(audio_signal, audio_signal->length + (size_old - size));
	else
	  ags_audio_signal_stream_resize(audio_signal, audio_signal->length - (size - size_old));
      }

      list_audio_signal = list_audio_signal->next;
    }

    list_recall_id = list_recall_id->next;
  }
}

void
ags_channel_recall_id_set_stage(AgsChannel *output, guint group_id, guint stage, gboolean ommit_own_channel)
{
  AgsChannel *current;
  gboolean async_recall;

  if((AGS_AUDIO_ASYNC & AGS_AUDIO(output->audio)->flags) != 0)
    async_recall = TRUE;
  else
    async_recall = FALSE;

  switch(async_recall){
  case TRUE:
    current = output->next_pad;

    while(current != NULL){
      ags_recall_id_set_stage(ags_recall_id_find_group_id(current->recall_id, group_id),
			      stage);
      
      current = current->next_pad;
    }
    
    current = output->prev_pad;
    
    while(current != NULL){
      ags_recall_id_set_stage(ags_recall_id_find_group_id(current->recall_id, group_id),
			      stage);
      
      current = current->prev_pad;
    }

  case FALSE:
    if(ommit_own_channel)
      break;

    ags_recall_id_set_stage(ags_recall_id_find_group_id(output->recall_id, group_id),
			    stage);
  }
}

void
ags_channel_play(AgsChannel *channel, AgsRecallID *recall_id, gint stage, gboolean do_recall){
  AgsRecall *recall;
  GList *list, *list_next;
  
  if(do_recall)
    list = channel->recall;
  else
    list = channel->play;
  
  while(list != NULL){
    list_next = list->next;

    recall = AGS_RECALL(list->data);

    if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0 ||
       recall->recall_id->group_id != recall_id->group_id){
      list = list_next;
      continue;
    }

    if((AGS_RECALL_HIDE & (recall->flags)) == 0){
      if(stage == 0)
	ags_recall_run_pre(recall, channel->audio_channel);
      else if(stage == 1)
	ags_recall_run_inter(recall, channel->audio_channel);
      else
	ags_recall_run_post(recall, channel->audio_channel);
    }

    ags_recall_check_cancel(recall);

    ags_recall_child_check_remove(recall);

    if((AGS_RECALL_REMOVE & (recall->flags)) != 0){
      if(do_recall)
	channel->recall = g_list_remove(channel->recall, recall);
      else
	channel->play = g_list_remove(channel->play, recall);

      ags_recall_remove(recall);
    }
    
    list = list_next;
  }
}

void
ags_channel_recursive_play(AgsChannel *channel, guint group_id, gint stage)
{
  AgsRecallID *recall_id;
  auto void ags_channel_recursive_play_output(AgsChannel *output, AgsRecallID *output_recall_id, gboolean do_recall);
  void ags_channel_recursive_play_get_input_recall_id(AgsChannel *input,
						      guint group_id,
						      gboolean find_parent_group_id,
						      AgsRecallID **input_recall_id){
    if(find_parent_group_id){
      (*input_recall_id) = ags_recall_id_find_parent_group_id(input->recall_id, group_id);
    }else{
      (*input_recall_id) = ags_recall_id_find_group_id(input->recall_id, group_id);
    }
  }
  void ags_channel_recursive_play_check_child_group_id(AgsAudio *audio,
						       AgsChannel *input, AgsRecallID *input_recall_id, gboolean input_do_recall,
						       AgsRecallID **child_recall_id, gboolean *child_do_recall){
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(audio)->flags)) != 0){
      (*child_recall_id) = ags_recall_id_find_parent_group_id(input->link->recall_id, input_recall_id->group_id);
      (*child_do_recall) = TRUE;
    }else{
      (*child_recall_id) = ags_recall_id_find_group_id(input->link->recall_id, input_recall_id->group_id);
      (*child_do_recall) = input_do_recall;
    }
  }
  void ags_channel_recursive_play_input_sync(AgsChannel *input, AgsRecallID *input_recall_id,
					     gboolean do_recall){
    AgsAudio *audio;
    AgsRecallID *child_recall_id;
    gboolean child_do_recall;

    audio = AGS_AUDIO(input->audio);

    /* check if we go down */
    if(input->link != NULL){
      /* check if there is a new group_id */
      ags_channel_recursive_play_check_child_group_id(audio,
						      input, input_recall_id, do_recall,
						      &child_recall_id, &child_do_recall);
      
      /* go down */
      ags_channel_recursive_play_output(input->link, child_recall_id,
					child_do_recall);
    }
      
    /* call input */
    ags_channel_play(input, input_recall_id, stage, do_recall);
  }
  void ags_channel_recursive_play_input_async(AgsChannel *input,
					      guint output_group_id,
					      gboolean do_recall, gboolean input_has_new_group_id){
    AgsRecallID *input_recall_id;

      /* iterate AgsInputs */
      while(input != NULL){
	/* get AgsRecallID of AgsInput */
	ags_channel_recursive_play_get_input_recall_id(input,
						       output_group_id,
						       input_has_new_group_id,
						       &input_recall_id);
	
	ags_channel_recursive_play_input_sync(input, input_recall_id,
					      do_recall);
	
	input = input->next_pad;
      }
  }
  void ags_channel_recursive_play_output(AgsChannel *output, AgsRecallID *output_recall_id,
					 gboolean do_recall){
    AgsAudio *audio;
    AgsChannel *current, *input;
    AgsRecallID *input_recall_id;
    gboolean input_do_recall, input_has_new_group_id;
    
    audio = AGS_AUDIO(output->audio);
    
    /* check if the AgsOutput's group_id is the same of AgsInput */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
      input_has_new_group_id = TRUE;
    else
      input_has_new_group_id = FALSE;
    
    /* check if input_do_recall */
    if(do_recall || (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
      input_do_recall = TRUE;
    else
      input_do_recall = FALSE;
    
    /* call input */
    if((AGS_AUDIO_SYNC & (audio->flags)) != 0){
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	if(ags_recall_id_get_run_stage(output_recall_id, stage)){
	  /* block sync|async for this run */
	  ags_channel_recall_id_set_stage(output, output_recall_id->group_id, stage, TRUE);

	  /* run in AGS_AUDIO_ASYNC mode */
	  ags_channel_recursive_play_input_async(ags_channel_nth(audio->input, output->audio_channel),
						 output_recall_id->group_id,
						 input_do_recall, input_has_new_group_id);
	}else{
	  /* unblock sync|async for next run */
	  if((AGS_CHANNEL_RUNNING & (current->flags)) != 0){
	    ags_recall_id_unset_stage(output_recall_id, stage);
	  }
	}
      }else{
	AgsChannel *input;
	
	input = ags_channel_nth(audio->input, output->line);
	
	ags_channel_recursive_play_get_input_recall_id(input,
						       output_recall_id->group_id,
						       input_has_new_group_id,
						       &input_recall_id);
	  
	ags_channel_recursive_play_input_sync(input, input_recall_id,
					      input_do_recall);
	
      }
    }else{
      /* run in AGS_AUDIO_ASYNC mode */
      ags_channel_recursive_play_input_async(ags_channel_nth(audio->input, output->audio_channel),
					     output_recall_id->group_id,
					     input_do_recall, input_has_new_group_id);
    }
    
    
    /* call audio */
    ags_audio_play(audio, output->audio_channel, group_id, stage, do_recall);
    
    /* call output */
    ags_channel_play(output, output_recall_id, stage, do_recall);
  }
  
  recall_id = ags_recall_id_find_group_id(channel->recall_id, group_id);
  
  if(AGS_IS_OUTPUT(channel)){
    ags_channel_recursive_play_output(channel, recall_id, FALSE);
  }else{
    ags_channel_recursive_play_input_sync(channel, recall_id, FALSE);
  }
}

void
ags_channel_recursive_play_init(AgsChannel *channel, gint stage,
				gboolean arrange_group_id, gboolean duplicate_templates,
				guint group_id, guint child_group_id)
{
  AgsAudio *audio;
  AgsRecycling *first_recycling, *last_recycling;
  gboolean stage_stop;
  auto void ags_channel_recursive_play_init_arrange_group_id_down(AgsChannel *output, guint parent_group_id, guint group_id, guint child_group_id, guint recall_level);
  auto void ags_channel_recursive_play_init_duplicate_down(AgsChannel *output, guint group_id);
  auto void ags_channel_recursive_play_init_down(AgsChannel *output, guint group_id);
  void ags_channel_recursive_play_init_arrange_group_id_up(AgsChannel *output,
							   AgsRecycling *first_recycling, AgsRecycling *last_recycling){
    AgsAudio *audio;
    AgsChannel *current;
    AgsRecallID *recall_id;
    
    current = channel;
    
    if(AGS_IS_OUTPUT(channel))
      goto ags_channel_recursive_play_init_up0;
    else
      audio = AGS_AUDIO(current->audio);
    
    /* goto toplevel AgsChannel */
    while(current != NULL){
      /* AgsInput */
      current->recall_id = ags_recall_id_add(current->recall_id,
					     0, group_id, child_group_id,
					     first_recycling, last_recycling,
					     FALSE);

      /* AgsAudio */
      audio->recall_id = ags_recall_id_add(audio->recall_id,
					   0, group_id, child_group_id,
					   first_recycling, last_recycling,
					   FALSE);

      /* AgsOutput */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
	break;

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0)
	current = ags_channel_nth(audio->output, current->audio_channel);
      else
	current = ags_channel_nth(audio->output, current->line);
    
    ags_channel_recursive_play_init_up0:
      current->recall_id = ags_recall_id_add(current->recall_id,
					     0, group_id, child_group_id,
					     first_recycling, last_recycling,
					     FALSE);

      /* iterate */
      if(current->link == NULL)
	break;
    
      audio = AGS_AUDIO(current->link->audio);
      
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
	break;
      
      current = current->link;
    }
  }
  void ags_channel_recursive_play_init_arrange_group_id_down_input(AgsChannel *output,
								   guint parent_group_id, guint group_id, guint child_group_id,
								   guint recall_level){
    AgsAudio *audio;
    AgsChannel *input;
    guint next_child_parent_group_id, next_child_group_id, next_child_child_group_id;
    guint next_child_recall_level;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      input = ags_channel_nth(audio->input, output->audio_channel);

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	next_child_parent_group_id = group_id;
	next_child_group_id = child_group_id;

	next_child_child_group_id = ags_recall_id_generate_group_id();

	next_child_recall_level = recall_level + 1;
      }else{
	next_child_parent_group_id = parent_group_id;
	next_child_group_id = group_id;
	next_child_child_group_id = child_group_id;

	next_child_recall_level = recall_level;
      }
    
      while(input != NULL){
	if(parent_group_id == 0)
	  input->recall_id = ags_recall_id_add(input->recall_id,
					       parent_group_id, group_id, child_group_id,
					       first_recycling, last_recycling,
					       FALSE);
	else{
	  

	  input->recall_id = ags_recall_id_add(input->recall_id,
					       parent_group_id, group_id, child_group_id,
					       input->first_recycling, input->last_recycling,
					       ((recall_level > 1) ? TRUE: FALSE));
	}

	if(input->link != NULL){
	  ags_channel_recursive_play_init_arrange_group_id_down(input->link,
								next_child_parent_group_id, next_child_group_id, next_child_child_group_id,
								next_child_recall_level);
	}

	input = input->next_pad;
      }
    }else{
      input = ags_channel_nth(audio->input, output->line);

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	next_child_parent_group_id = group_id;
	next_child_group_id = child_group_id;

	next_child_child_group_id = ags_recall_id_generate_group_id();

	next_child_recall_level = recall_level + 1;
      }else{
	next_child_parent_group_id = parent_group_id;
	next_child_group_id = group_id;
	next_child_child_group_id = child_group_id;

	next_child_recall_level = recall_level;
      }

      if(parent_group_id == 0)
	input->recall_id = ags_recall_id_add(input->recall_id,
					     parent_group_id, group_id, child_group_id,
					     first_recycling, last_recycling,
					     FALSE);
      else
	input->recall_id = ags_recall_id_add(input->recall_id,
					     parent_group_id, group_id, child_group_id,
					     input->first_recycling, input->last_recycling,
					     ((recall_level > 1) ? TRUE: FALSE));

      if(input->link != NULL){
	ags_channel_recursive_play_init_arrange_group_id_down(input->link,
							      next_child_parent_group_id, next_child_group_id, next_child_child_group_id,
							      next_child_recall_level);
      }
    }
  }
  void ags_channel_recursive_play_init_arrange_group_id_down(AgsChannel *output,
							     guint parent_group_id, guint group_id, guint child_group_id,
							     guint recall_level){
    AgsAudio *audio;

    /* AgsOutput */
    if(parent_group_id == 0)
      output->recall_id = ags_recall_id_add(output->recall_id,
					    parent_group_id, group_id, child_group_id,
					    first_recycling, last_recycling,
					    FALSE);
    else{
      output->recall_id = ags_recall_id_add(output->recall_id,
					    parent_group_id, group_id, child_group_id,
					    output->first_recycling, output->last_recycling,
					    ((recall_level > 1) ? TRUE: FALSE));
    }

    /* AgsAudio */
    audio = AGS_AUDIO(output->audio);
    
    audio->recall_id = ags_recall_id_add(audio->recall_id,
					 parent_group_id, group_id, child_group_id,
					 first_recycling, last_recycling,
					 ((recall_level > 1) ? TRUE: FALSE));
    
    /* AgsInput */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
      ags_channel_recursive_play_init_arrange_group_id_down_input(output,
								  group_id, child_group_id, ags_recall_id_generate_group_id(),
								  recall_level + 1);
    else
      ags_channel_recursive_play_init_arrange_group_id_down_input(output,
								  parent_group_id, group_id, child_group_id,
								  recall_level);
  }
  void ags_channel_recursive_play_init_duplicate_channel_recall(AgsChannel *channel, guint group_id)
  {
    AgsRecall *recall;
    AgsRecallID *recall_id;
    GList *list_recall;

    recall_id = ags_recall_id_find_group_id(channel->recall_id, group_id);

    if(recall_id->parent_group_id == 0)
      list_recall = channel->play;
    else
      list_recall = channel->recall;

    while(list_recall != NULL){
      recall = AGS_RECALL(list_recall->data);

      if((AGS_RECALL_RUN_INITIALIZED & (recall->flags)) != 0 ||
	 AGS_IS_RECALL_CHANNEL(recall)){
	list_recall = list_recall->next;
	continue;
      }

      if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
	recall = ags_recall_duplicate(recall, recall_id);
	//	recall->recall_id = recall_id;
	ags_connectable_connect(AGS_CONNECTABLE(recall));
	
	if(recall_id->parent_group_id == 0)
	  channel->play = g_list_prepend(channel->play, recall);
	else
	  channel->recall = g_list_prepend(channel->recall, recall);
      }
  
      list_recall = list_recall->next;
    }
  }
  void ags_channel_recursive_play_init_duplicate_audio_recall(AgsAudio *audio, guint audio_channel, guint group_id)
  {
    AgsRecall *recall;
    AgsRecallID *recall_id;
    GList *list_recall_start, *list_recall;

    recall_id = ags_recall_id_find_group_id(audio->recall_id, group_id);

    if(recall_id->parent_group_id == 0)
      list_recall_start = 
	list_recall = audio->play;
    else
      list_recall_start =
	list_recall = audio->recall;

    while(list_recall != NULL){
      recall = AGS_RECALL(list_recall->data);

      if((AGS_RECALL_RUN_INITIALIZED & (recall->flags)) != 0 ||
	 AGS_IS_RECALL_AUDIO(recall)){
	list_recall = list_recall->next;
	continue;
      }

      if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
	GList *list_recall_run_notify;
	
	/* check if the object has already been duplicated */
	list_recall_run_notify = ags_recall_find_type_with_group_id(list_recall_start, G_OBJECT_TYPE(recall), group_id);
	
	if(list_recall_run_notify != NULL){
	  /* notify additional run on the recall */
	  recall = AGS_RECALL(list_recall_run_notify->data);
	  
	  ags_recall_notify_dependency(recall, AGS_RECALL_NOTIFY_RUN, 1);
	}else{
	  /* duplicate the recall, notify first run and initialize it */
	  recall = ags_recall_duplicate(recall, recall_id);
	  //	  recall->recall_id = recall_id;
	  ags_connectable_connect(AGS_CONNECTABLE(recall));
	  
	  if(recall_id->parent_group_id == 0)
	    audio->play = g_list_prepend(audio->play, recall);
	  else
	    audio->recall = g_list_prepend(audio->recall, recall);
	  
	  ags_recall_notify_dependency(recall, AGS_RECALL_NOTIFY_RUN, 1);
	}
      }

      list_recall = list_recall->next;
    }
  }
  void ags_channel_recursive_play_init_duplicate_up(AgsChannel *channel)
  {
    AgsAudio *audio;
    AgsChannel *current;
    AgsRecallID *recall_id;
    
    current = channel;
    
    if(AGS_IS_OUTPUT(channel))
      goto ags_channel_recursive_play_init_duplicate_up0;
    else
      audio = AGS_AUDIO(current->audio);
    
    /* goto toplevel AgsChannel */
    while(current != NULL){
      /* AgsInput */
      ags_channel_recursive_play_init_duplicate_channel_recall(current, group_id);
      
      /* AgsAudio */
      ags_channel_recursive_play_init_duplicate_audio_recall(audio, current->audio_channel, group_id);

      /* AgsOutput */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
	break;

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0)
	current = ags_channel_nth(audio->output, current->audio_channel);
      else
	current = ags_channel_nth(audio->output, current->line);
    
    ags_channel_recursive_play_init_duplicate_up0:
      ags_channel_recursive_play_init_duplicate_channel_recall(current, group_id);

      /* iterate */      
      if(current->link == NULL)
	break;
    
      audio = AGS_AUDIO(current->link->audio);
      
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
	break;
      
      current = current->link;
    }
  }
  void ags_channel_recursive_play_init_duplicate_down_input(AgsChannel *output,
							    guint group_id){
    AgsAudio *audio;
    AgsChannel *input;
    guint next_group_id;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      input = ags_channel_nth(audio->input, output->audio_channel);
    
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	next_group_id = ags_recall_id_find_group_id(input->recall_id, group_id)->child_group_id;
      }else{
	next_group_id = group_id;
      }
      
      while(input != NULL){
	if(input->link != NULL){
	  ags_channel_recursive_play_init_duplicate_down(input->link,
							 next_group_id);
	}

	ags_channel_recursive_play_init_duplicate_channel_recall(input, group_id);

	input = input->next_pad;
      }
    }else{
      input = ags_channel_nth(audio->input, output->line);

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	next_group_id = ags_recall_id_find_group_id(input->recall_id, group_id)->child_group_id;
      }else{
	next_group_id = child_group_id;
      }

      if(input->link != NULL){
	ags_channel_recursive_play_init_duplicate_down(input->link,
						       next_group_id);
      }

      ags_channel_recursive_play_init_duplicate_channel_recall(input, group_id);
    }
  }
  void ags_channel_recursive_play_init_duplicate_down(AgsChannel *output,
						      guint group_id){
    AgsAudio *audio;

    /* AgsAudio */
    audio = AGS_AUDIO(output->audio);

    ags_channel_recursive_play_init_duplicate_audio_recall(audio, output->audio_channel, group_id);
    
    /* AgsInput */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
      ags_channel_recursive_play_init_duplicate_down_input(output,
							   ags_recall_id_find_group_id(output->recall_id, group_id)->child_group_id);
    else
      ags_channel_recursive_play_init_duplicate_down_input(output,
						 group_id);

    ags_channel_recursive_play_init_duplicate_channel_recall(output, group_id);
  }
  void ags_channel_recursive_play_init_channel_recall(AgsChannel *channel, guint group_id)
  {
    AgsRecall *recall;
    AgsRecallID *recall_id;
    GList *list_recall;

    recall_id = ags_recall_id_find_group_id(channel->recall_id, group_id);

    if(recall_id->parent_group_id == 0)
      list_recall = channel->play;
    else
      list_recall = channel->recall;

    while(list_recall != NULL){
      recall = AGS_RECALL(list_recall->data);

      if((AGS_RECALL_RUN_INITIALIZED & (recall->flags)) != 0 ||
	  AGS_IS_RECALL_CHANNEL(recall)){
	list_recall = list_recall->next;
	continue;
      }

      if((AGS_RECALL_TEMPLATE & (recall->flags)) == 0){
	if(stage == 0){
	  recall->flags &= (~AGS_RECALL_HIDE);
	  ags_recall_run_init_pre(recall, channel->audio_channel);
	  recall->flags &= (~AGS_RECALL_REMOVE);
	}else if(stage == 1){
	  ags_recall_run_init_inter(recall, channel->audio_channel);
	}else{
	  ags_recall_run_init_post(recall, channel->audio_channel);

	  ags_run_connectable_connect(AGS_RUN_CONNECTABLE(recall));

	  recall->flags |= AGS_RECALL_RUN_INITIALIZED;
	}
      }
  
      list_recall = list_recall->next;
    }
  }
  void ags_channel_recursive_play_init_audio_recall(AgsAudio *audio, guint audio_channel, guint group_id)
  {
    AgsRecall *recall;
    AgsRecallID *recall_id;
    GList *list_recall;

    recall_id = ags_recall_id_find_group_id(audio->recall_id, group_id);

    if(recall_id->parent_group_id == 0)
      list_recall = audio->play;
    else
      list_recall = audio->recall;

    while(list_recall != NULL){
      recall = AGS_RECALL(list_recall->data);

      if((AGS_RECALL_RUN_INITIALIZED & (recall->flags)) != 0 ||
	  AGS_IS_RECALL_AUDIO(recall)){
	list_recall = list_recall->next;
	continue;
      }

      if((AGS_RECALL_TEMPLATE & (recall->flags)) == 0){
	if(stage == 0){
	  recall->flags &= (~AGS_RECALL_HIDE);
	  ags_recall_run_init_pre(recall, audio_channel);
	  recall->flags &= (~AGS_RECALL_REMOVE);
	}else if(stage == 1){
	  ags_recall_run_init_inter(recall, audio_channel);
	}else{
	  ags_recall_run_init_post(recall, audio_channel);
	    
	  ags_run_connectable_connect(AGS_RUN_CONNECTABLE(recall));
	  
	  recall->flags |= AGS_RECALL_RUN_INITIALIZED;
	}
      }
  
      list_recall = list_recall->next;
    }
  }
  void ags_channel_recursive_play_init_up(AgsChannel *channel)
  {
    AgsAudio *audio;
    AgsChannel *current;
    AgsRecallID *recall_id;
    
    current = channel;
    
    if(AGS_IS_OUTPUT(channel))
      goto ags_channel_recursive_play_init_up0;
    else
      audio = AGS_AUDIO(current->audio);
    
    /* goto toplevel AgsChannel */
    while(current != NULL){
      /* AgsInput */
      ags_channel_recursive_play_init_channel_recall(current, group_id);
      
      /* AgsAudio */
      ags_channel_recursive_play_init_audio_recall(audio, current->audio_channel, group_id);

      /* AgsOutput */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
	break;

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0)
	current = ags_channel_nth(audio->output, current->audio_channel);
      else
	current = ags_channel_nth(audio->output, current->line);
    
    ags_channel_recursive_play_init_up0:
      ags_channel_recursive_play_init_channel_recall(current, group_id);

      /* iterate */      
      if(current->link == NULL)
	break;
    
      audio = AGS_AUDIO(current->link->audio);
      
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
	break;
      
      current = current->link;
    }
  }
  void ags_channel_recursive_play_init_down_input(AgsChannel *output,
						  guint group_id){
    AgsAudio *audio;
    AgsChannel *input;
    guint next_group_id;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      input = ags_channel_nth(audio->input, output->audio_channel);
    
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	next_group_id = ags_recall_id_find_group_id(input->recall_id, group_id)->child_group_id;
      }else{
	next_group_id = group_id;
      }
      
      while(input != NULL){
	if(input->link != NULL){
	  ags_channel_recursive_play_init_down(input->link,
					       next_group_id);
	}

	ags_channel_recursive_play_init_channel_recall(input, group_id);

	input = input->next_pad;
      }
    }else{
      input = ags_channel_nth(audio->input, output->line);

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	next_group_id = ags_recall_id_find_group_id(input->recall_id, group_id)->child_group_id;
      }else{
	next_group_id = child_group_id;
      }

      if(input->link != NULL){
	ags_channel_recursive_play_init_down(input->link,
					     next_group_id);
      }

      ags_channel_recursive_play_init_channel_recall(input, group_id);
    }
  }
  void ags_channel_recursive_play_init_down(AgsChannel *output,
					    guint group_id){
    AgsAudio *audio;

    /* AgsAudio */
    audio = AGS_AUDIO(output->audio);

    ags_channel_recursive_play_init_audio_recall(audio, output->audio_channel, group_id);
    
    /* AgsInput */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
      ags_channel_recursive_play_init_down_input(output,
						 ags_recall_id_find_group_id(output->recall_id, group_id)->child_group_id);
    else
      ags_channel_recursive_play_init_down_input(output,
						 group_id);

    ags_channel_recursive_play_init_channel_recall(output, group_id);
  }

  /* entry point */
  first_recycling = channel->first_recycling;
  last_recycling = channel->last_recycling;

  audio = AGS_AUDIO(channel->audio);

  /* arrange group_id */
  if(arrange_group_id){
    if(AGS_IS_OUTPUT(channel)){
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	guint tmp_group_id;

	tmp_group_id = ags_recall_id_generate_group_id();
	 
	ags_channel_recursive_play_init_arrange_group_id_down_input(channel,
								    group_id, child_group_id, tmp_group_id,
								    1);
      }else{
	ags_channel_recursive_play_init_arrange_group_id_down_input(channel,
								    0, group_id, child_group_id,
								    0);
      }
      
      audio->recall_id = ags_recall_id_add(audio->recall_id,
					   0, group_id, child_group_id,
					   first_recycling, last_recycling,
					   FALSE);
    }else{
      if(channel->link != NULL){
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  ags_channel_recursive_play_init_arrange_group_id_down(channel->link,
								group_id, child_group_id, ags_recall_id_generate_group_id(),
								1);
	}else{
	  ags_channel_recursive_play_init_arrange_group_id_down_input(channel->link,
								      0, group_id, child_group_id,
								      0);
	}
      }
    }

    ags_channel_recursive_play_init_arrange_group_id_up(channel,
							first_recycling, last_recycling);
  }

  /* duplicate AgsRecall templates */
  if(duplicate_templates){
    if(AGS_IS_OUTPUT(channel)){
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){	
	ags_channel_recursive_play_init_duplicate_down_input(channel,
							     child_group_id);
      }else{
	ags_channel_recursive_play_init_duplicate_down_input(channel,
						   group_id);
      }
      
      ags_channel_recursive_play_init_duplicate_audio_recall(audio, channel->audio_channel, group_id);
    }else{
      if(channel->link != NULL){
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  ags_channel_recursive_play_init_duplicate_down(channel->link,
							 child_group_id);
	}else{
	  ags_channel_recursive_play_init_duplicate_down_input(channel->link,
							       group_id);
	}
      }
    }

    ags_channel_recursive_play_init_duplicate_up(channel);
  }

  /* do the different stages of recall initializiation */
  if(stage == -1){
    stage = 0;
    stage_stop = 3;
  }else
    stage_stop = stage + 1;

  for(; stage < stage_stop; stage++){
    if(AGS_IS_OUTPUT(channel)){
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){	
	ags_channel_recursive_play_init_down_input(channel,
						   child_group_id);
      }else{
	ags_channel_recursive_play_init_down_input(channel,
						   group_id);
      }
      
      ags_channel_recursive_play_init_audio_recall(audio, channel->audio_channel, group_id);
    }else{
      if(channel->link != NULL){
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  ags_channel_recursive_play_init_down(channel->link,
					       child_group_id);
	}else{
	  ags_channel_recursive_play_init_down_input(channel->link,
						     group_id);
	}
      }
    }

    ags_channel_recursive_play_init_up(channel);
  }
}

AgsChannel*
ags_channel_new()
{
  AgsChannel *channel;

  channel = (AgsChannel *) g_object_new(AGS_TYPE_CHANNEL, NULL);

  return(channel);
}
