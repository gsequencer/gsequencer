/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/ags_channel.h>

#include <ags/main.h>

#include <ags/lib/ags_list.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_marshal.h>

#include <ags/thread/ags_recycling_thread.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_run_order.h>

#include <stdio.h>

void ags_channel_class_init(AgsChannelClass *channel_class);
void ags_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_channel_init(AgsChannel *channel);
void ags_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec);
void ags_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec);
void ags_channel_add_to_registry(AgsConnectable *connectable);
void ags_channel_remove_from_registry(AgsConnectable *connectable);
void ags_channel_connect(AgsConnectable *connectable);
void ags_channel_disconnect(AgsConnectable *connectable);
static void ags_channel_finalize(GObject *gobject);

enum{
  RECYCLING_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_DEVOUT,
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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_channel = g_type_register_static(G_TYPE_OBJECT,
					      "AgsChannel\0",
					      &ags_channel_info, 0);

    g_type_add_interface_static(ags_type_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_channel);
}

void
ags_channel_class_init(AgsChannelClass *channel)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_channel_parent_class = g_type_class_peek_parent(channel);

  /* GObjectClass */
  gobject = (GObjectClass *) channel;

  gobject->set_property = ags_channel_set_property;
  gobject->get_property = ags_channel_get_property;

  gobject->finalize = ags_channel_finalize;

  /* properties */
  param_spec = g_param_spec_object("audio\0",
				   "assigned audio\0",
				   "The audio it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  param_spec = g_param_spec_object("devout\0",
				   "assigned devout\0",
				   "The devout it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  /* AgsChannelClass */
  channel->recycling_changed = NULL;

  /* signals */
  channel_signals[RECYCLING_CHANGED] =
    g_signal_new("recycling_changed\0",
		 G_TYPE_FROM_CLASS (channel),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsChannelClass, recycling_changed),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__OBJECT_OBJECT_OBJECT_OBJECT_OBJECT_OBJECT_OBJECT_OBJECT,
		 G_TYPE_NONE, 8,
		 G_TYPE_OBJECT, G_TYPE_OBJECT,
		 G_TYPE_OBJECT, G_TYPE_OBJECT,
		 G_TYPE_OBJECT, G_TYPE_OBJECT,
		 G_TYPE_OBJECT, G_TYPE_OBJECT);
}

void
ags_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->add_to_registry = ags_channel_add_to_registry;
  connectable->remove_from_registry = ags_channel_remove_from_registry;
  connectable->connect = ags_channel_connect;
  connectable->disconnect = ags_channel_disconnect;
}

GQuark
ags_channel_error_quark()
{
  return(g_quark_from_static_string("ags-channel-error-quark\0"));
}

void
ags_channel_init(AgsChannel *channel)
{
  channel->flags = 0;

  channel->audio = NULL;
  channel->devout = NULL;

  channel->prev = NULL;
  channel->prev_pad = NULL;
  channel->next = NULL;
  channel->next_pad = NULL;

  channel->pad = 0;
  channel->audio_channel = 0;
  channel->line = 0;

  channel->note = NULL;

  channel->devout_play = ags_devout_play_alloc();
  AGS_DEVOUT_PLAY(channel->devout_play)->source = (GObject *) channel;

  channel->recycling_container_root = NULL;
  channel->recall_id = NULL;
  channel->container = NULL;

  channel->recall = NULL;
  channel->play = NULL;

  channel->link = NULL;
  channel->first_recycling = NULL;
  channel->last_recycling = NULL;

  channel->recycling_thread = (GObject *) ags_recycling_thread_new();

  channel->pattern = NULL;
  channel->notation = NULL;

  channel->line_widget = NULL;
  channel->file_data = NULL;
}


void
ags_channel_set_property(GObject *gobject,
			 guint prop_id,
			 const GValue *value,
			 GParamSpec *param_spec)
{
  AgsChannel *channel;

  channel = AGS_CHANNEL(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      GObject *audio;

      audio = g_value_get_object(value);

      if(channel->audio == audio)
	return;

      if(audio != NULL)
	g_object_ref(audio);

      channel->audio = audio;
    }
    break;
  case PROP_DEVOUT:
    {
      AgsDevout *devout;

      devout = (AgsDevout *) g_value_get_object(value);

      ags_channel_set_devout(channel, devout);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_channel_get_property(GObject *gobject,
			 guint prop_id,
			 GValue *value,
			 GParamSpec *param_spec)
{
  AgsChannel *channel;

  channel = AGS_CHANNEL(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    g_value_set_object(value, channel->audio);
    break;
  case PROP_DEVOUT:
    g_value_set_object(value, channel->devout);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_channel_add_to_registry(AgsConnectable *connectable)
{
  AgsMain *ags_main;
  AgsServer *server;
  AgsChannel *channel;
  AgsRegistryEntry *entry;
  GList *list;
  
  channel = AGS_CHANNEL(connectable);

  ags_main = AGS_MAIN(AGS_DEVOUT(AGS_AUDIO(channel->audio)->devout)->ags_main);

  server = ags_main->server;

  entry = ags_registry_entry_alloc(server->registry);
  g_value_set_object(&(entry->entry),
		     (gpointer) channel);
  ags_registry_add(server->registry,
		   entry);

  /* add play */
  list = channel->play;

  while(list != NULL){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
  
  /* add recall */
  list = channel->recall;

  while(list != NULL){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_channel_remove_from_registry(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_channel_connect(AgsConnectable *connectable)
{
  AgsChannel *channel;
  AgsRecycling *recycling;
  GList *list;

  channel = AGS_CHANNEL(connectable);

  ags_connectable_add_to_registry(connectable);

  /* connect recall ids */
  list = channel->recall_id;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recall containers */
  list = channel->container;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recalls */
  list = channel->recall;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  list = channel->play;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recycling */
  recycling = channel->first_recycling;

  while(recycling != channel->last_recycling->next){
    ags_connectable_connect(AGS_CONNECTABLE(recycling));

    recycling = recycling->next;
  }

  /* connect pattern and notation */
  list = channel->pattern;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  if(channel->notation != NULL)
    ags_connectable_connect(AGS_CONNECTABLE(channel->notation));
}

void
ags_channel_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
  /* empty */
}

static void
ags_channel_finalize(GObject *gobject)
{
  AgsChannel *channel;
  AgsRecycling *recycling, *recycling_next;

  channel = AGS_CHANNEL(gobject);

  if(channel->audio != NULL)
    g_object_unref(channel->audio);

  /* AgsRecycling */
  if(((AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0 && AGS_IS_INPUT(channel)) ||
     ((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0 && AGS_IS_OUTPUT(channel))){

    recycling = channel->first_recycling;

    if(recycling != NULL)
      while(recycling != channel->last_recycling->next){
	recycling_next = recycling->next;

	g_object_unref((GObject *) recycling);

	recycling = recycling_next;
      }
  }

  /* key string */
  if(channel->note != NULL)
    free(channel->note);

  if(channel->devout_play != NULL)
    ags_devout_play_free(channel->devout_play);  


  /* free some lists */
  ags_list_free_and_unref_link(channel->recall_id);
  ags_list_free_and_unref_link(channel->container);
  
  ags_list_free_and_unref_link(channel->recall);
  ags_list_free_and_unref_link(channel->play);

  ags_list_free_and_unref_link(channel->pattern);

  /* AgsNotation */
  if(channel->notation != NULL)
    g_object_unref(channel->notation);

  /* call parent class */
  G_OBJECT_CLASS(ags_channel_parent_class)->finalize(gobject);
}

AgsRecall*
ags_channel_find_recall(AgsChannel *channel, char *effect, char *name)
{
  /* */
}

/**
 * ags_channel_first:
 * @channel an #AgsChannel
 * Returns: the first #AgsChannel
 *
 * Iterates until the first #AgsChannel was found.
 */
AgsChannel*
ags_channel_first(AgsChannel *channel)
{
  channel = ags_channel_pad_first(channel);

  while(channel->prev != NULL)
    channel = channel->prev;

  return(channel);
}

/**
 * ags_channel_last:
 * @channel an #AgsChannel
 * Returns: the last #AgsChannel
 *
 * Iterates until the last #AgsChannel was found.
 */
AgsChannel*
ags_channel_last(AgsChannel *channel)
{
  channel = ags_channel_pad_last(channel);

  while(channel->next != NULL)
    channel = channel->next;

  return(channel);
}

/**
 * ags_channel_nth:
 * @channel an #AgsChannel
 * @nth the count to iterate
 * Returns: the nth #AgsChannel
 *
 * Iterates @nth times forward.
 */
AgsChannel*
ags_channel_nth(AgsChannel *channel, guint nth)
{
  guint i;

  for(i = 0; i < nth && channel != NULL; i++){
    channel = channel->next;
  }

  if((nth != 0 && i != nth) || channel == NULL)
    g_message("ags_channel_nth:\n  nth channel does not exist\n  `- stopped @: i = %u; nth = %u\n\0", i, nth);

  return(channel);
}

/**
 * ags_channel_pad_first:
 * @channel an #AgsChannel
 * Returns: the first #AgsChannel with the same audio_channel as @channel
 *
 * Iterates until the first pad has been reached.
 */
AgsChannel*
ags_channel_pad_first(AgsChannel *channel)
{
  while(channel->prev_pad != NULL)
    channel = channel->prev_pad;

  return(channel);
}

/**
 * ags_channel_pad_last:
 * @channel an #AgsChannel
 * Returns: the last #AgsChannel with the same audio_channel as @channel
 *
 * Iterates until the last pad has been reached.
 */
AgsChannel*
ags_channel_pad_last(AgsChannel *channel)
{
  while(channel->next_pad != NULL)
    channel = channel->next_pad;

  return(channel);
}

/**
 * ags_channel_pad_nth:
 * @channel an #AgsChannel
 * @nth the count of pads to step
 * Returns: the nth pad
 *
 * Iterates nth times.
 */
AgsChannel*
ags_channel_pad_nth(AgsChannel *channel, guint nth)
{
  guint i;

  for(i = 0; i < nth && channel != NULL; i++)
    channel = channel->next_pad;

  if((nth != 0 && i != nth) || channel == NULL)
    g_message("ags_channel_nth_pad:\n  nth pad does not exist\n  `- stopped @: i = %u; nth = %u\0", i, nth);

  return(channel);
}

/**
 * ags_channel_first_with_recycling:
 * @channel an #AgsChannel
 * Returns: the first channel with an #AgsRecycling
 *
 * Iterates over pads forwards as long as there is no #AgsRecycling on the #AgsChannel.
 */
AgsChannel*
ags_channel_first_with_recycling(AgsChannel *channel)
{
  channel = ags_channel_pad_first(channel);

  while(channel != NULL){
    if(channel->first_recycling != NULL)
      break;

    channel = channel->next_pad;
  }

  return(channel);
}

/**
 * ags_channel_last_with_recycling:
 * @channel an #AgsChannel
 * Returns: the last channel with an #AgsChannel.
 *
 * Iterates over pads backwards as long as there is no #AgsRecycling on the #AgsChannel.
 */
AgsChannel*
ags_channel_last_with_recycling(AgsChannel *channel)
{
  channel = ags_channel_pad_last(channel);

  while(channel != NULL){
    if(channel->last_recycling != NULL)
      break;

    channel = channel->prev_pad;
  }

  return(channel);
}

void
ags_channel_set_devout(AgsChannel *channel, GObject *devout)
{
  GList *list;

  /* channel */
  if(channel->devout == devout)
    return;

  if(channel->devout != NULL)
    g_object_unref(channel->devout);

  if(devout != NULL)
    g_object_ref(devout);

  channel->devout = (GObject *) devout;

  /* recall */
  list = channel->play;

  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "devout\0", devout,
		 NULL);

    list = list->next;
  }

  list = channel->recall;

  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "devout\0", devout,
		 NULL);

    list = list->next;
  }

  /* AgsRecycling */
  if((AGS_IS_OUTPUT(channel) &&
      (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0) ||
     ((AGS_IS_INPUT(channel) &&
       (AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0))){
    AgsRecycling *recycling;

    recycling = channel->first_recycling;

    while(recycling != channel->last_recycling->next){
      g_object_set(G_OBJECT(recycling),
		   "devout\0", devout,
		   NULL); 

      recycling = recycling->next;
    }
  }
}

void
ags_channel_remove_recall_id(AgsChannel *channel, AgsRecallID *recall_id)
{
  /*
   * TODO:JK: thread synchronisation
   */

  channel->recall_id = g_list_remove(channel->recall_id,
				     recall_id);
  g_object_unref(G_OBJECT(recall_id));
}

void
ags_channel_add_recall_id(AgsChannel *channel, AgsRecallID *recall_id)
{
  /*
   * TODO:JK: thread synchronisation
   */

  g_object_ref(G_OBJECT(recall_id));

  channel->recall_id = g_list_prepend(channel->recall_id,
				      recall_id);
}

void
ags_channel_add_recall_container(AgsChannel *channel, GObject *container)
{
  /*
   * TODO:JK: thread synchronisation
   */

  g_object_ref(G_OBJECT(container));

  channel->container = g_list_prepend(channel->container, container);
}

void
ags_channel_remove_recall(AgsChannel *channel, GObject *recall, gboolean play)
{
  /*
   * TODO:JK: thread synchronisation
   */

  if(play){
    channel->play = g_list_remove(channel->play, recall);
  }else{
    channel->recall = g_list_remove(channel->recall, recall);
  }

  g_object_unref(G_OBJECT(recall));
}

void
ags_channel_add_recall(AgsChannel *channel, GObject *recall, gboolean play)
{
  /*
   * TODO:JK: thread synchronisation
   */
  g_object_ref(G_OBJECT(recall));

  if(play){
    channel->play = g_list_append(channel->play, recall);
  }else{
    channel->recall = g_list_append(channel->recall, recall);
  }
}

/**
 * ags_channel_set_link:
 * @channel an #AgsChannel to link
 * @link an other #AgsChannel to link with
 * @error you may retrieve a AGS_CHANNEL_ERROR_LOOP_IN_LINK error
 *
 * Change the linking of #AgsChannel objects.
 */
//FIXME:JK: you shouldn't link AgsInput with own AgsRecycling
void
ags_channel_set_link(AgsChannel *channel, AgsChannel *link,
		     GError **error)
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

  /* check for AgsRecycling in input */

  /* check for a loop */
  if(link != NULL){
    AgsAudio *audio, *current_audio;
    AgsChannel *current_channel;

    audio = AGS_AUDIO(channel->audio);

    current_channel = link;
    current_audio = AGS_AUDIO(current_channel->audio);

    while(TRUE){
      if(current_audio == audio){
	if(link != NULL)
	  g_set_error(error,
		      AGS_CHANNEL_ERROR,
		      AGS_CHANNEL_ERROR_LOOP_IN_LINK,
		      "failed to link channel %u from %s with channel %u from %s\0",
		      channel->line, G_OBJECT_TYPE_NAME(audio),
		      link->line, G_OBJECT_TYPE_NAME(link->audio));
	else
	  g_set_error(error,
		      AGS_CHANNEL_ERROR,
		      AGS_CHANNEL_ERROR_LOOP_IN_LINK,
		      "failed to link channel %u from %s with channel %u from %s\0",
		      channel->line, G_OBJECT_TYPE_NAME(audio),
		      0, "NULL\0");

	return;
      }

      if(current_channel->link == NULL)
	break;

      current_audio = AGS_AUDIO(current_channel->link->audio);

      if((AGS_AUDIO_ASYNC & (current_audio->flags)) != 0)
	current_channel = ags_channel_nth(current_audio->output, current_channel->audio_channel)->link;
      else
	current_channel = ags_channel_nth(current_audio->output, current_channel->line)->link;
    }
  }

  /* link AgsInput */
  if(input != NULL){
    input_link = input->link;
    input->link = output;
  }else
    input_link = NULL;

  if(input_link != NULL){
    input_link->link = NULL;
  }

  /* link AgsOutput */
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
	recycling->parent = output->first_recycling;

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
	recycling->parent = output->first_recycling;

      recycling = recycling->next;
    }
  }

  /* reset recall_id */
  ags_channel_recursive_reset_recall_ids(input, output,
					 input_link, output_link);
}

/**
 * ags_channel_set_recycling:
 * @channel the channel to reset
 * @first_recycling the recycling to set for channel->first_recycling
 * @last_recycling the recycling to set for channel->last_recycling
 * @update reset allthough the AgsRecyclings are still the same
 * @destroy_old destroy old AgsRecyclings
 *
 * Called by ags_channel_set_link() to handle outdated #AgsRecycling references.
 */
void
ags_channel_set_recycling(AgsChannel *channel,
			  AgsRecycling *first_recycling, AgsRecycling *last_recycling,
			  gboolean update, gboolean destroy_old)
{
  AgsAudio *audio;
  AgsAudio *found_prev, *found_next;
  AgsRecycling *old_first_recycling, *old_last_recycling;
  AgsRecycling *replace_with_first_recycling, *replace_with_last_recycling;
  AgsRecycling *changed_old_first_recycling, *changed_old_last_recycling;
  AgsChannel *prev_channel, *next_channel;
  AgsRecycling *nth_recycling, *next_recycling, *stop_recycling;
  AgsRecycling *parent;
  gboolean replace_first, replace_last;
  gboolean find_prev, find_next;
  gboolean change_old_last, change_old_first;

  auto gboolean ags_channel_set_recycling_recursive_input(AgsChannel *input);
  auto void ags_channel_set_recycling_recursive_output(AgsChannel *output);
  auto void ags_channel_set_recycling_recursive(AgsChannel *input);
  auto void ags_channel_set_recycling_emit_changed_input(AgsChannel *input);
  auto void ags_channel_set_recycling_emit_changed_output(AgsChannel *output);
  auto gboolean ags_channel_set_recycling_emit_changed(AgsChannel *input);

  gboolean ags_channel_set_recycling_recursive_input(AgsChannel *input){
    AgsChannel *nth_channel_prev, *nth_channel_next;

    /* update input AgsRecallIDs */
    ags_recall_id_reset_recycling(input->recall_id,
				  input->first_recycling,
				  replace_with_first_recycling, replace_with_last_recycling);

    /* update audio AgsRecallIDs */
    ags_recall_id_reset_recycling(audio->recall_id,
				  input->first_recycling,
				  replace_with_first_recycling, replace_with_last_recycling);

    if(replace_first){
      /* set recycling */
      input->first_recycling = replace_with_first_recycling;
    }

    if(replace_last){
      /* set recycling */
      input->last_recycling = replace_with_last_recycling;
    }

    if((AGS_AUDIO_ASYNC & (AGS_AUDIO(input->audio)->flags)) != 0){
      /* search for neighboor recyclings */
      if(find_prev){
	nth_channel_prev = input->prev_pad;

	while(nth_channel_prev != NULL &&
	      nth_channel_prev->last_recycling == NULL){
	  nth_channel_prev = nth_channel_prev->prev_pad;
	}

	if(nth_channel_prev != NULL){
	  if(replace_first){
	    found_prev = audio;
	    prev_channel = nth_channel_prev;
	  }

	  replace_first = FALSE;
	}
      }

      if(find_next){
	nth_channel_next = input->next_pad;

	while(nth_channel_next != NULL &&
	      nth_channel_next->first_recycling == NULL){
	  nth_channel_next = nth_channel_next->next_pad;
	}

	if(nth_channel_next != NULL){
	  if(replace_last){
	    found_next = audio;
	    next_channel = nth_channel_next;
	  }

	  replace_last = FALSE;
	}
      }

      /* check if there's still a need for finding neighboor recyclings */
      if(!replace_first)
	if(!replace_last){
	  find_next = FALSE;
	  find_prev = FALSE;

	  return(TRUE);
	}else{
	  if(first_recycling == NULL){
	    /* reset to NULL */
	    replace_with_last_recycling = nth_channel_prev->last_recycling;
	  }else
	    find_prev = FALSE;
	}
    }else if(!replace_last){
      if(first_recycling == NULL){
	/* reset to NULL */
	replace_with_first_recycling = nth_channel_next->first_recycling;
      }else
	find_next = FALSE;
    }

    return(FALSE);
  }
  void ags_channel_set_recycling_recursive_output(AgsChannel *output){
    /* update input AgsRecallIDs */
    ags_recall_id_reset_recycling(output->recall_id,
				  output->first_recycling,
				  replace_with_first_recycling, replace_with_last_recycling);

    if(replace_first)
      output->first_recycling = replace_with_first_recycling;

    if(replace_last)
      output->last_recycling = replace_with_last_recycling;

    if(output->link != NULL)
      ags_channel_set_recycling_recursive(output->link);
  }
  void ags_channel_set_recycling_recursive(AgsChannel *input){
    AgsAudio *audio;
    AgsChannel *output;

    audio = AGS_AUDIO(input->audio);

    /* AgsInput */
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
      gboolean abort;
      
      abort = ags_channel_set_recycling_recursive_input(input);
      
      if(abort){
	return;
      }
    }else
      return;

    /* AgsOutput */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0)
	output = ags_channel_nth(audio->output, input->audio_channel);
      else
	output = ags_channel_nth(audio->output, input->line);

      ags_channel_set_recycling_recursive_output(output);
    }
  }
  void ags_channel_set_recycling_emit_changed_input(AgsChannel *input){
    AgsChannel *nth_channel;
    AgsAudio *audio;

    audio = AGS_AUDIO(input->audio);

    if(audio == found_prev){
      change_old_first = TRUE;
    }

    if(audio == found_next){
      change_old_last = TRUE;
    }

    /* emit changed */
    ags_channel_recycling_changed(input,
				  changed_old_first_recycling, changed_old_last_recycling,
				  input->first_recycling, input->last_recycling,
				  old_first_recycling, old_last_recycling,
				  first_recycling, last_recycling);

    /* change will affect output */
    if(change_old_first){
      nth_channel = ags_channel_first_with_recycling(input);

      if(nth_channel == NULL){
	changed_old_first_recycling = NULL;
	changed_old_last_recycling = NULL;
	return;
      }

      changed_old_first_recycling = nth_channel->first_recycling;
    }

    if(change_old_last){
      nth_channel = ags_channel_last_with_recycling(input);
      
      changed_old_last_recycling = nth_channel->last_recycling;
    }
  }
  void ags_channel_set_recycling_emit_changed_output(AgsChannel *output){
    ags_channel_recycling_changed(output,
				  changed_old_first_recycling, changed_old_last_recycling,
				  output->first_recycling, output->last_recycling,
				  old_first_recycling, old_last_recycling,
				  first_recycling, last_recycling);

    if(output->link != NULL)
      if(ags_channel_set_recycling_emit_changed(output->link))
	parent = output->first_recycling;
    else
      parent = output->first_recycling;
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

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      output = ags_channel_nth(audio->output, input->audio_channel);
    }else{
      output = ags_channel_nth(audio->output, input->line);
    }
      
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0)
      ags_channel_set_recycling_emit_changed_output(output);
    else
      parent = output->first_recycling;

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

  found_prev = NULL;
  found_next = NULL;

  parent = NULL;

  prev_channel = NULL;
  next_channel = NULL;

  replace_with_first_recycling = first_recycling;
  replace_with_last_recycling = last_recycling;

  changed_old_first_recycling = old_first_recycling;
  changed_old_last_recycling = old_last_recycling;

  replace_first = TRUE;
  replace_last = TRUE;

  find_next = TRUE;
  find_prev = TRUE;

  change_old_first = FALSE;
  change_old_last = FALSE;

  if((old_first_recycling == first_recycling)){
    if(!update){
      if(old_last_recycling == last_recycling)
	return;

      replace_first = FALSE;
    }
  }

  if((old_last_recycling == last_recycling)){
    if(!update)
      replace_last = FALSE;
  }

  /* set recycling */
  if(AGS_IS_INPUT(channel)){
    AgsChannel *output;

    if(!ags_channel_set_recycling_recursive_input(channel)){
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
	if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	  output = ags_channel_nth(audio->output, channel->audio_channel);
	}else{
	  output = ags_channel_nth(audio->output, channel->line);
	}

	ags_channel_set_recycling_recursive_output(output);	
      }
    }
  }else{
    ags_channel_set_recycling_recursive_output(channel);
  }

  /* join now the retrieved recyclings */
  if(first_recycling != NULL){
    if(prev_channel != NULL &&
       prev_channel->last_recycling != NULL){
      prev_channel->last_recycling->next = first_recycling;
      first_recycling->prev = prev_channel->last_recycling;
    }else{
      first_recycling->prev = NULL;
    }
    
    if(next_channel != NULL &&
       next_channel->first_recycling != NULL){
      next_channel->first_recycling->prev = last_recycling;
      last_recycling->next = next_channel->first_recycling;
    }else{
      last_recycling->next = NULL;
    }
  }else{
    gboolean link_next, link_prev;

    if(prev_channel != NULL &&
       prev_channel->last_recycling != NULL)
      link_next = TRUE;
    else
      link_next = FALSE;

    if(next_channel != NULL &&
       next_channel->first_recycling != NULL)
      link_prev = TRUE;
    else
      link_prev = FALSE;
    
    if(link_next){
      if(link_prev){
	next_channel->first_recycling->prev = prev_channel->last_recycling;
	prev_channel->last_recycling->next = next_channel->first_recycling;
      }else{
	prev_channel->last_recycling->next = NULL;
      }
    }else if(link_prev){
      next_channel->first_recycling->prev = NULL;
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

    output = ags_channel_nth(audio->output, (((AGS_AUDIO_ASYNC & (audio->flags)) == 0) ? channel->line: channel->audio_channel));
    
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0)
      ags_channel_set_recycling_emit_changed_output(output);
    else
      parent = output->first_recycling;
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
			      AgsRecycling *new_start_region, AgsRecycling *new_end_region,
			      AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
			      AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region)
{
  g_return_if_fail(AGS_IS_CHANNEL(channel));

  g_object_ref(G_OBJECT(channel));
  g_signal_emit(G_OBJECT(channel),
		channel_signals[RECYCLING_CHANGED], 0,
		old_start_region, old_end_region,
		new_start_region, new_end_region,
		old_start_changed_region, old_end_changed_region,
		new_start_changed_region, new_end_changed_region);
  g_object_unref(G_OBJECT(channel));
}

GObject*
ags_channel_find_recycling_container_root(AgsChannel *channel)
{
  AgsAudio *audio;

  if(AGS_IS_OUTPUT(channel)){
    goto ags_channel_find_recycling_container_root_OUTPUT;
  }

  while(channel != NULL){
    /* input */
    if(channel->recycling_root != NULL){
      return(channel);
    }

    /* audio */
    audio = AGS_AUDIO(channel->audio);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      channel = ags_channel_nth(audio->output,
				channel->audio_channel);
    }else{
      channel = ags_channel_nth(audio->output,
				channel->line);
    }

  ags_channel_find_recycling_container_root_OUTPUT:
    /* output */
    if(channel->recycling_root != NULL){
      return(channel);
    }
    
    /* iterate */
    channel = channel->link;
  }
  
  return(NULL);
}

GObject*
ags_channel_find_recycling_container(AgsChannel *channel)
{
  AgsRecyclingContainer *recycling_container_root;

  auto GObject* ags_channel_find_recycling_container_recursive(AgsRecyclingContainer *recycling_container,
							       AgsRecycling *recycling);

  GObject* ags_channel_find_recycling_container_recursive(AgsRecyclingContainer *recycling_container,
							  AgsRecycling *recycling){
    AgsRecyclingContainer *child, *retval;
    gint i;

    for(i = 0; i < recycling_container->length; i++){
      if(recycling_container->recycling[i] == recycling){
	return(recycling_container);
      }
    }

    child = recycling_container->children;

    while(child != NULL){
      retval = ags_channel_find_recycling_container_recursive(child,
							      recycling);

      if(retval != NULL){
	return(retval);
      }

      child = child->next;
    }

    return(NULL);
  }

  recycling_container_root = ags_channel_find_recycling_container_root(channel);

  if(recycling_container_root == NULL){
    return(NULL);
  }

  return(ags_channel_find_recycling_container_recursive(recycling_container_root, channel->first_recycling));
}

void
ags_channel_safe_resize_audio_signal(AgsChannel *channel,
				     guint length)
{
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;
  GList *list;
  guint new_length;
  gdouble factor;

  recycling = channel->first_recycling;

  while(recycling != channel->last_recycling->next){
    list = recycling->audio_signal;

    while(list != NULL){
      audio_signal = AGS_AUDIO_SIGNAL(list->data);

      if(audio_signal->stream_current != NULL){
	if(length < audio_signal->length){
	  factor = audio_signal->length / length;

	  new_length = length + (factor *
	    g_list_position(audio_signal->stream_beginning,
			    audio_signal->stream_current));
	}else{
	  factor = length / audio_signal->length;

	  new_length = length - (factor *
	    g_list_position(audio_signal->stream_beginning,
			    audio_signal->stream_current));
	}
      }else{
	new_length = length;
      }

      ags_audio_signal_stream_safe_resize(audio_signal,
					  new_length);

      list = list->next;
    }

    recycling = recycling->next;
  }
}

void
ags_channel_resolve_recall(AgsChannel *channel,
			   AgsRecallID *recall_id)
{
  AgsRecall *recall;
  GList *list_recall;

  /* get the appropriate lists */
  if(recall_id->parent == NULL){
    list_recall = channel->play;
  }else{
    list_recall = channel->recall;
  }
  
  while((list_recall = ags_recall_find_recall_id(list_recall, recall_id)) != NULL){
    recall = AGS_RECALL(list_recall->data);

    ags_recall_resolve_dependencies(recall);

    list_recall = list_recall->next;
  }
}

void
ags_channel_play(AgsChannel *channel, AgsRecallID *recall_id, gint stage, gboolean do_recall)
{
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
       recall->recall_id == NULL ||
       recall->recall_id->recycling_container != recall_id->recycling_container){
      list = list_next;

      continue;
    }

    if((AGS_RECALL_HIDE & (recall->flags)) == 0){
      if(stage == 0)
	ags_recall_run_pre(recall);
      else if(stage == 1)
	ags_recall_run_inter(recall);
      else
	ags_recall_run_post(recall);
    }

    list = list_next;
  }
}

/**
 * ags_channel_recursive_play_threaded:
 * @channel
 * @recall_id
 * @stage
 *
 *
 */
void
ags_channel_recursive_play_threaded(AgsChannel *channel,
				    AgsRecallID *recall_id, gint stage)
{
  auto void ags_channel_recursive_play_input_sync_threaded(AgsChannel *input, AgsRecallID *input_recall_id);
  auto void ags_channel_recursive_play_input_async_threaded(AgsChannel *input,
							    AgsRecallID output_recall_id);
  auto void ags_channel_recursive_play_output_threaded(AgsChannel *output, AgsRecallID *output_recall_id);
  auto void ags_channel_recursive_play_up_threaded(AgsChannel *channel, AgsRecallID *recall_id);

  void ags_channel_recursive_play_input_sync_threaded(AgsChannel *input, AgsRecallID *input_recall_id){
    AgsAudio *audio;
    AgsRecallID *child_recall_id;
    gboolean child_do_recall;

    audio = AGS_AUDIO(input->audio);

    /* check if we go down */
    if(input->link != NULL){
      gint child_position;

      /* check if there is a new recycling */
      child_position = ags_recycling_container_find_child(input_recall_id->recycling_container,
							  input->first_recycling);
      
      if(child_position == -1){
	child_recall_id = input_recall_id;
      }else{
	GList *list;

	list = g_list_nth(input_recall_id->children,
			  child_position);

	if(list != NULL){
	  child_recall_id = AGS_RECYCLING_CONTAINER(list->data)->recall_id;
	}else{
	  child_recall_id = NULL;
	}
      }

      /* go down */
      ags_channel_recursive_play_output_threaded(input->link, child_recall_id,
						 child_do_recall);
    }
      
    /* call input */
    ags_recycling_thread_play_channel(AGS_RECYCLING_THREAD(AGS_CHANNEL(input)->recycling_thread),
				      G_OBJECT(input),
				      input_recall_id,
				      stage, do_recall);

    ags_iterator_thread_children_ready(AGS_ITERATOR_THREAD(AGS_RECYCLING_THREAD(input->recycling_thread)->iterator_thread),
				       AGS_THREAD(input->recycling_thread));
  }
  void ags_channel_recursive_play_input_async_threaded(AgsChannel *input,
						       AgsRecallID *output_recall_id){
    AgsAudio *audio;
    AgsRecallID *input_recall_id, *child_recall_id;

    /* iterate AgsInputs */
    while(input != NULL){
      gint child_position;

      /* input_recall_id - check if there is a new recycling */
      child_position = ags_recycling_container_find_child(output_recall_id->recycling_container,
							  input->first_recycling);
      
      if(child_position == -1){
	input_recall_id = output_recall_id;
      }else{
	GList *list;

	list = g_list_nth(output_recall_id->children,
			  child_position);

	if(list != NULL){
	  input_recall_id = AGS_RECYCLING_CONTAINER(list->data)->recall_id;
	}else{
	  input_recall_id = NULL;
	}
      }

      /* check if we go down */
      if(input->link != NULL){
	/* child_recall_id - check if there is a recycling */
	child_position = ags_recycling_container_find_child(input_recall_id->recycling_container,
							    input->link->first_recycling);
      
	if(child_position == -1){
	  child_recall_id = input_recall_id;
	}else{
	  GList *list;

	  list = g_list_nth(output_recall_id->children,
			    child_position);

	  if(list != NULL){
	    child_recall_id = AGS_RECYCLING_CONTAINER(list->data)->recall_id;
	  }else{
	    child_recall_id = NULL;
	  }
	}

	/* go down */
	ags_channel_recursive_play_output_threaded(input->link,
						   child_recall_id);
      }
      
      /* call input */
      ags_recycling_thread_play_channel(AGS_RECYCLING_THREAD(AGS_CHANNEL(input)->recycling_thread),
					G_OBJECT(input),
					input_recall_id,
					stage);
	
      input = input->next_pad;
    }

    ags_iterator_thread_children_ready(AGS_ITERATOR_THREAD(AGS_RECYCLING_THREAD(input->recycling_thread)->iterator_thread),
				       AGS_THREAD(input->recycling_thread));
  }
  void ags_channel_recursive_play_output_threaded(AgsChannel *output, AgsRecallID *output_recall_id){
    AgsAudio *audio;
    AgsRecallID *input_recall_id;
    gint child_position;
    
    audio = AGS_AUDIO(output->audio);
    
    /* input_recall_id - check if there is a new recycling */
    child_position = ags_recycling_container_find_child(output_recall_id->recycling_container,
							input->first_recycling);
      
    if(child_position == -1){
      input_recall_id = output_recall_id;
    }else{
      GList *list;

      list = g_list_nth(output_recall_id->children,
			child_position);

      if(list != NULL){
	input_recall_id = AGS_RECYCLING_CONTAINER(list->data)->recall_id;
      }else{
	input_recall_id = NULL;
      }
    }

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
	  ags_channel_recall_id_set(output, output_recall_id, TRUE,
				    AGS_CHANNEL_RECALL_ID_RUN_STAGE,
				    "stage\0", stage,
				    NULL);

	  /* run in AGS_AUDIO_ASYNC mode */
	  ags_channel_recursive_play_input_async_threaded(ags_channel_nth(audio->input, output->audio_channel),
							  output_recall_id);
	}else{
	  /* unblock sync|async for next run */
	  if((AGS_CHANNEL_RUNNING & (output->flags)) != 0){
	    ags_recall_id_unset_run_stage(output_recall_id, stage);
	  }
	}

	/* call audio */
	if(child_recall_id->recycling_container != output_recall_id->recycling_container){
	  AgsRecycling *first_recycling, *last_recycling;

	  first_recycling = ags_output_find_first_input_recycling(AGS_OUTPUT(output));
	  last_recycling = ags_output_find_last_input_recycling(AGS_OUTPUT(output));

	  ags_recycling_thread_play_audio(AGS_RECYCLING_THREAD(AGS_CHANNEL(output)->recycling_thread),
					  G_OBJECT(output), G_OBJECT(audio),
					  input_recall_id,
					  stage);
	}

	ags_recycling_thread_play_audio(AGS_RECYCLING_THREAD(AGS_CHANNEL(output)->recycling_thread),
					G_OBJECT(output), G_OBJECT(audio),
					output_recall_id,
					stage);
      }else{
	AgsChannel *input;

	input = ags_channel_nth(audio->input, output->line);
	
	/* traverse the tree */
	ags_channel_recursive_play_input_sync_threaded(input, input_recall_id);

	/* call audio */
	if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  ags_recycling_thread_play_audio(AGS_RECYCLING_THREAD(output->recycling_thread),
					  G_OBJECT(output), G_OBJECT(audio),
					  output_recall_id,
					  stage);
	}

	ags_recycling_thread_play_audio(AGS_RECYCLING_THREAD(output->recycling_thread),
					G_OBJECT(output), G_OBJECT(audio),
					output_recall_id,
					stage);
      }
    }else{
      /* run in AGS_AUDIO_ASYNC mode */
      ags_channel_recursive_play_input_async_threaded(ags_channel_nth(audio->input, output->audio_channel),
						      input_recall_id);

      /* call audio */
      if(input_recall_id->recycling_container != output_recall_id->recycling_container){
	AgsRecycling *first_recycling, *last_recycling;
	
	first_recycling = ags_output_find_first_input_recycling(AGS_OUTPUT(output));
	last_recycling = ags_output_find_last_input_recycling(AGS_OUTPUT(output));

	ags_recycling_thread_play_audio(AGS_RECYCLING_THREAD(output->recycling_thread),
					G_OBJECT(output), G_OBJECT(audio),
					input_recall_id,
					stage);
      }

      ags_recycling_thread_play_audio(AGS_RECYCLING_THREAD(output->recycling_thread),
				      G_OBJECT(output), G_OBJECT(audio),
				      output_recall_id,
				      stage);
    }
    
    /* call output */
    ags_recycling_thread_play_channel(AGS_RECYCLING_THREAD(output->recycling_thread),
				      G_OBJECT(output),
				      output_recall_id,
				      stage);

    ags_iterator_thread_children_ready(AGS_ITERATOR_THREAD(AGS_RECYCLING_THREAD(output->recycling_thread)->iterator_thread),
				       AGS_THREAD(output->recycling_thread));
  }
  void ags_channel_recursive_play_up_threaded(AgsChannel *channel, AgsRecallID *recall_id){
    AgsAudio *audio;

    if(channel == NULL)
      return;

    audio = AGS_AUDIO(channel->audio);
    
    if(AGS_IS_INPUT(channel)){
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;

      goto ags_channel_recursive_play_up0;
    }else{
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;
      
      goto ags_channel_recursive_play_up1;
    }
    
    while(channel != NULL){
      audio = AGS_AUDIO(channel->audio);

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;

      /* play input */
    ags_channel_recursive_play_up0:
      ags_recycling_thread_play_channel(AGS_RECYCLING_THREAD(channel->recycling_thread),
					G_OBJECT(channel),
					recall_id,
					stage);

      ags_iterator_thread_children_ready(AGS_ITERATOR_THREAD(AGS_RECYCLING_THREAD(channel->recycling_thread)->iterator_thread),
					 AGS_THREAD(channel->recycling_thread));

      /* play audio */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;

      /* get output */
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0)
	channel = ags_channel_nth(audio->output, channel->audio_channel);
      else
	channel = ags_channel_nth(audio->output, channel->line);

      ags_recycling_thread_play_audio(AGS_RECYCLING_THREAD(channel->recycling_thread),
				      G_OBJECT(channel), G_OBJECT(audio),
				      channel->first_recycling, channel->last_recycling,
				      recall_id,
				      stage);

      /* play output */
    ags_channel_recursive_play_up1:
      ags_recycling_thread_play_channel(AGS_RECYCLING_THREAD(channel->recycling_thread),
					G_OBJECT(channel),
					recall_id,
					stage);

      ags_iterator_thread_children_ready(AGS_ITERATOR_THREAD(AGS_RECYCLING_THREAD(channel->recycling_thread)->iterator_thread),
					 AGS_THREAD(channel->recycling_thread));

      channel = channel->link;
    }
  }

  /* entry point */
  if(AGS_IS_OUTPUT(channel)){
    ags_channel_recursive_play_output_threaded(channel,
					       recall_id);
  }else{
    ags_channel_recursive_play_input_sync_threaded(channel,
						   recall_id);
  }

  ags_channel_recursive_play_up_threaded(channel->link,
					 recall_id);
}

/**
 * ags_channel_recursive_play:
 * @channel
 * @recall_id
 * @stage
 *
 *
 */
void
ags_channel_recursive_play(AgsChannel *channel,
			   AgsRecallID *recall_id, gint stage)
{
  AgsRecallID *recall_id;

  auto void ags_channel_recursive_play_input_sync(AgsChannel *input, AgsRecallID *input_recall_id);
  auto void ags_channel_recursive_play_input_async(AgsChannel *input,
						   AgsRecallID *output_recall_id);
  auto void ags_channel_recursive_play_output(AgsChannel *output, AgsRecallID *output_recall_id);
  auto void ags_channel_recursive_play_up(AgsChannel *channel, AgsRecallID *recall_id);

  void ags_channel_recursive_play_input_sync(AgsChannel *input, AgsRecallID *input_recall_id){
    AgsAudio *audio;
    AgsRecallID *child_recall_id;
    gboolean child_do_recall;

    if(input == NULL){
      return;
    }

    audio = AGS_AUDIO(input->audio);

    /* check if we go down */
    if(input->link != NULL){
      gint child_position;

      /* check if there is a new recycling */
      child_position = ags_recycling_container_find_child(input_recall_id->recycling_container,
							  input->first_recycling);
      
      if(child_position == -1){
	child_recall_id = input_recall_id;
      }else{
	GList *list;

	list = g_list_nth(input_recall_id->children,
			  child_position);

	if(list != NULL){
	  child_recall_id = AGS_RECYCLING_CONTAINER(list->data)->recall_id;
	}else{
	  child_recall_id = NULL;
	}
      }
      
      /* go down */
      ags_channel_recursive_play_output(input->link, child_recall_id);
    }
      
    /* call input */
    ags_channel_play(input, input_recall_id, stage);
  }
  void ags_channel_recursive_play_input_async(AgsChannel *input,
					      AgsRecallID *output_recall_id){
    AgsRecallID *input_recall_id;

    if(input == NULL){
      return;
    }

    /* iterate AgsInputs */
    while(input != NULL){
      gint child_position;

      /* input_recall_id - check if there is a new recycling */
      child_position = ags_recycling_container_find_child(output_recall_id->recycling_container,
							  input->first_recycling);
      
      if(child_position == -1){
	input_recall_id = output_recall_id;
      }else{
	GList *list;

	list = g_list_nth(output_recall_id->children,
			  child_position);

	if(list != NULL){
	  input_recall_id = AGS_RECYCLING_CONTAINER(list->data)->recall_id;
	}else{
	  input_recall_id = NULL;
	}
      }

      /* go down */	
      ags_channel_recursive_play_input_sync(input, input_recall_id);

      /* iterate */
      input = input->next_pad;
    }
  }
  void ags_channel_recursive_play_output(AgsChannel *output, AgsRecallID *output_recall_id){
    AgsAudio *audio;
    AgsRecallID *input_recall_id;
    gint child_position;
    
    if(output == NULL){
      return;
    }

    audio = AGS_AUDIO(output->audio);
    

    /* input_recall_id - check if there is a new recycling */
    child_position = ags_recycling_container_find_child(output_recall_id->recycling_container,
							input->first_recycling);
      
    if(child_position == -1){
      input_recall_id = output_recall_id;
    }else{
      GList *list;

      list = g_list_nth(output_recall_id->children,
			child_position);

      if(list != NULL){
	input_recall_id = AGS_RECYCLING_CONTAINER(list->data)->recall_id;
      }else{
	input_recall_id = NULL;
      }
    }
    
    /* call input */
    if((AGS_AUDIO_SYNC & (audio->flags)) != 0){
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	if(ags_recall_id_get_run_stage(output_recall_id, stage)){
	  /* block sync|async for this run */
	  ags_channel_recall_id_set(output, output_recall_id, TRUE,
				    AGS_CHANNEL_RECALL_ID_RUN_STAGE,
				    "stage\0", stage,
				    NULL);

	  /* traverse the tree - run in AGS_AUDIO_ASYNC mode */
	  ags_channel_recursive_play_input_async(ags_channel_nth(audio->input, output->audio_channel),
						 output_recall_id);
	}else{
	  /* unblock sync|async for next run */
	  if((AGS_CHANNEL_RUNNING & (output->flags)) != 0){
	    ags_recall_id_unset_run_stage(output_recall_id, stage);
	  }
	}

	/* call audio */
	if(input_recall_id->recycling_container != output_recall_id->recycling_container){
	  AgsRecycling *first_recycling, *last_recycling;

	  first_recycling = ags_output_find_first_input_recycling(AGS_OUTPUT(output));
	  last_recycling = ags_output_find_last_input_recycling(AGS_OUTPUT(output));

	  ags_audio_play(audio,
			 first_recycling, last_recycling,
			 input_recall_id,
			 stage);
	}
	
	ags_audio_play(audio,
		       output->first_recycling, output->last_recycling,
		       output_recall_id,
		       stage);
      }else{
	AgsChannel *input;
	
	input = ags_channel_nth(audio->input, output->line);
		  
	ags_channel_recursive_play_input_sync(input, input_recall_id,
					      input_do_recall);

	/* call audio */
	if(input_recall_id->recycling_container != output_recall_id->recycling_container){
	  AgsRecycling *first_recycling, *last_recycling;

	  first_recycling = ags_output_find_first_input_recycling(AGS_OUTPUT(output));
	  last_recycling = ags_output_find_last_input_recycling(AGS_OUTPUT(output));

	  ags_audio_play(audio,
			 first_recycling, last_recycling,
			 input_recall_id,
			 stage);
	}

	ags_audio_play(audio,
		       output->first_recycling, output->last_recycling,
		       output_recall_id,
		       stage);
      }
    }else{
      /* run in AGS_AUDIO_ASYNC mode */
      ags_channel_recursive_play_input_async(ags_channel_nth(audio->input, output->audio_channel),
					     output_recall_id);

      /* call audio */
      if(input_recall_id->recycling_container != output_recall_id->recycling_container){
	AgsRecycling *first_recycling, *last_recycling;
	
	first_recycling = ags_output_find_first_input_recycling(AGS_OUTPUT(output));
	last_recycling = ags_output_find_last_input_recycling(AGS_OUTPUT(output));

	ags_audio_play(audio,
		       first_recycling, last_recycling,
		       input_recall_id,
		       stage);
      }

      ags_audio_play(audio,
		     output->first_recycling, output->last_recycling,
		     output_recall_id,
		     stage);
    }
    
    /* call output */
    ags_channel_play(output, output_recall_id, stage, do_recall);
  }
  void ags_channel_recursive_play_up(AgsChannel *channel, AgsRecallID *recall_id){
    AgsAudio *audio;

    if(channel == NULL)
      return;

    audio = AGS_AUDIO(channel->audio);
    
    if(AGS_IS_INPUT(channel)){
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;

      goto ags_channel_recursive_play_up0;
    }else{
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;
      
      goto ags_channel_recursive_play_up1;
    }
    
    while(channel != NULL){
      audio = AGS_AUDIO(channel->audio);

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;

      /* play input */
    ags_channel_recursive_play_up0:
      ags_channel_play(channel,
		       recall_id,
		       stage);

      /* play audio */
      ags_audio_play(audio,
		     channel->first_recycling, channel->last_recycling,
		     recall_id,
		     stage);

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;

      /* get output */
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0)
	channel = ags_channel_nth(audio->output, channel->audio_channel);
      else
	channel = ags_channel_nth(audio->output, channel->line);

      /* play output */
    ags_channel_recursive_play_up1:
      ags_channel_play(channel,
		       recall_id,
		       stage);

      channel = channel->link;
    }
  }

  /* entry point */
  if(AGS_IS_OUTPUT(channel)){
    ags_channel_recursive_play_output(channel,
				      recall_id);
  }else{
    ags_channel_recursive_play_input_sync(channel,
					  recall_id);
  }

  ags_channel_recursive_play_up(channel->link,
				recall_id);
}

/**
 * ags_channel_duplicate_recall:
 * @channel an #AgsChannel that contains the #AgsRecall templates
 * @playback if a #AgsRecall that is dedicated to a playback should be duplicated
 * @sequencer if a #AgsRecall that is dedicated to a sequencer should be duplicated
 * @notation if a #AgsRecall that is dedicated to a notation should be duplicated
 * @group_id the #AgsGroupID the newly allocated #AgsRecall objects belongs to
 * @audio_signal_level how many parental #AgsAudioSignal this @group_id has
 *
 * Duplicate #AgsRecall templates for use with ags_channel_recursive_play(),
 * but ags_channel_recursive_play_init() may call this function for you.
 */
void
ags_channel_duplicate_recall(AgsChannel *channel,
			     gboolean playback, gboolean sequencer, gboolean notation,
			     AgsGroupId group_id,
			     guint audio_signal_level)
{
  AgsAudio *audio;
  AgsRecall *copy;
  AgsRunOrder *run_order;
  AgsRecallID *recall_id;
  AgsRecall *recall;
  GList *list_recall;
  gboolean immediate_new_level;
  
  recall_id = ags_recall_id_find_group_id(channel->recall_id, group_id);
  
  /* get the appropriate lists */
  if(recall_id->parent_group_id == 0){
    list_recall = channel->play;
  }else{
    list_recall = channel->recall;
  }

  audio = AGS_AUDIO(channel->audio);

  /*  */
  if(AGS_IS_OUTPUT(channel)){
    immediate_new_level = FALSE;

    /* run order */
    run_order = ags_run_order_find_group_id(audio->run_order,
					    recall_id->group_id);

    if(run_order == NULL){
      AgsRecallID *audio_recall_id;

      audio_recall_id = ags_recall_id_find_group_id(audio->recall_id,
						    recall_id->group_id);
  
      run_order = ags_run_order_new(audio_recall_id);
      ags_audio_add_run_order(audio, run_order);
    }

    if(AGS_IS_OUTPUT(channel) &&
       g_list_index(run_order->run_order, channel) == -1){
      ags_run_order_add_channel(run_order,
				channel);
    }    
  }else{
    AgsChannel *output;
    AgsGroupId group_id;

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      immediate_new_level = TRUE;
    }else{
      immediate_new_level = FALSE;
    }

    /* run order */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      if(recall_id->parent_group_id != 0){
	group_id = recall_id->parent_group_id;
      }else{
	group_id = recall_id->group_id;
      }
    }else{
      group_id = recall_id->group_id;
    }

    run_order = ags_run_order_find_group_id(audio->run_order,
					    group_id);

    if(run_order == NULL){
      AgsRecallID *audio_recall_id;

      audio_recall_id = ags_recall_id_find_group_id(audio->recall_id,
						    group_id);
  
      run_order = ags_run_order_new(audio_recall_id);
      ags_audio_add_run_order(audio, run_order);
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      output = ags_channel_nth(audio->output,
			       channel->audio_channel);
    }else{
      output = ags_channel_nth(audio->output,
			       channel->line);
    }

    if(g_list_index(run_order->run_order, output) == -1){
      ags_run_order_add_channel(run_order,
				output);
    }    

  }

  while(list_recall != NULL){
    recall = AGS_RECALL(list_recall->data);

    /* ignore initialized or non-runnable AgsRecalls */
    if((AGS_RECALL_TEMPLATE & (recall->flags)) == 0 ||
       AGS_IS_RECALL_AUDIO(recall) || AGS_IS_RECALL_CHANNEL(recall) ||
       !((playback && (AGS_RECALL_PLAYBACK & (recall->flags)) != 0) ||
	 (sequencer && (AGS_RECALL_SEQUENCER & (recall->flags)) != 0) ||
	 (notation && (AGS_RECALL_NOTATION & (recall->flags)) != 0))){
      list_recall = list_recall->next;
      continue;
    }
    
    /* duplicate the recall */
    copy = ags_recall_duplicate(recall, recall_id);
    g_message("duplicated: %s\n\0", G_OBJECT_TYPE_NAME(copy));
    
    /* set appropriate flag */
    if(playback){
      ags_recall_set_flags(copy, AGS_RECALL_PLAYBACK);
    }else if(sequencer){
      ags_recall_set_flags(copy, AGS_RECALL_SEQUENCER);
    }else if(notation){
      ags_recall_set_flags(copy, AGS_RECALL_NOTATION);
    }

    /* append to AgsAudio */
    if(recall_id->parent_group_id == 0)
      channel->play = g_list_append(channel->play, copy);
    else
      channel->recall = g_list_append(channel->recall, copy);
      
    /* connect */
    ags_connectable_connect(AGS_CONNECTABLE(copy));

    /* notify run */
    ags_recall_notify_dependency(recall, AGS_RECALL_NOTIFY_RUN, 1);

    /* iterate */    
    list_recall = list_recall->next;
  }
}

/**
 * ags_channel_init_recall:
 * @channel an #AgsChannel that contains the recalls
 * @group_id the #AgsGroupId this recall belongs to
 *
 * Prepare #AgsRecall objects to become runnning, ags_channel_recursive_play_init()
 * may call this function for you.
 */
void
ags_channel_init_recall(AgsChannel *channel, gint stage,
			AgsGroupId group_id)
{
  AgsRecall *recall;
  AgsRecallID *recall_id;
  GList *list_recall;
  
  recall_id = ags_recall_id_find_group_id(channel->recall_id, group_id);

  g_message("ags_channel_init_recall@%d - audio::IN[%u]; channel: %llu %llu\n\0", stage, AGS_AUDIO(channel->audio)->input_lines, (long long unsigned int) channel->audio_channel, (long long unsigned int) channel->pad);
  

  if(recall_id->parent_group_id == 0)
    list_recall = channel->play;
  else
    list_recall = channel->recall;

  while(list_recall != NULL){
    recall = AGS_RECALL(list_recall->data);
    
    if((AGS_RECALL_RUN_INITIALIZED & (recall->flags)) != 0 ||
       recall->recall_id == NULL ||
       recall->recall_id->group_id != group_id ||
       AGS_IS_RECALL_CHANNEL(recall)){
      list_recall = list_recall->next;
      continue;
    }
    
    if((AGS_RECALL_TEMPLATE & (recall->flags)) == 0){
      g_message("  init: %s\n\0", G_OBJECT_TYPE_NAME(recall));

      if(stage == 0){
	ags_dynamic_connectable_connect_dynamic(AGS_DYNAMIC_CONNECTABLE(recall));

	recall->flags &= (~AGS_RECALL_HIDE);
	ags_recall_run_init_pre(recall);
	recall->flags &= (~AGS_RECALL_REMOVE);
      }else if(stage == 1){
	ags_recall_run_init_inter(recall);
      }else{
	ags_recall_run_init_post(recall);
	
	//	  recall->flags |= AGS_RECALL_RUN_INITIALIZED;
      }
    }
    
    list_recall = list_recall->next;
  }
}

/**
 * ags_channel_recursive_play_init:
 * @channel the #AgsChannel to prepare
 * @stage valid values for @stage are: -1 for running all three stages, or the stages 0 through 2 to run
 * just the specified stage. With stage is meant the #AgsRecall::run_init_pre, #AgsRecall::run_init_inter
 * and #AgsRecall::run_init_post stages.
 * @arrange_group_id %TRUE if new #AgsRecallID objects should be created and therefor valid #AgsGroupId\s
 * @duplicate_templates %TRUE if the #AgsRecall templates should be duplicated
 * @playback %TRUE if the purpose is a simple playback of the tree, this option is used to omit the
 * duplication of #AgsRecall templates which haven't set @AGS_RECALL_PLAYBACK flag
 * @sequencer %TRUE if the purpose is playing the tree for a sequencer, this option is used to omit the
 * duplication of #AgsRecall templates which haven't set the @AGS_RECALL_SEQUENCER flag
 * @notation %TRUE if the purpose is playing the tree for a notation, this option is used to omit the
 * duplication of #AgsRecall templates which haven't set the @AGS_RECALL_NOTATION flag
 * @resolve_dependencies %TRUE if the
 * @group_id the initial group id
 * @child_group_id the initial child group id
 * @audio_signal_level the current audio signal level
 *
 * Make the tree ready for a new #AgsDevoutPlay.
 */
void
ags_channel_recursive_play_init(AgsChannel *channel, gint stage,
				gboolean arrange_group_id,
				gboolean duplicate_templates, gboolean playback, gboolean sequencer, gboolean notation,
				gboolean resolve_dependencies,
				AgsRecallID *recall_id)
{
  AgsAudio *audio;
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecallContainer *recall_container;
  gboolean stage_stop;

  auto void ags_channel_recursive_play_init_arrange_group_id_up(AgsChannel *channel,
								guint audio_signal_level);
  auto void ags_channel_recursive_play_init_arrange_group_id_down_input(AgsChannel *output,
									AgsGroupId parent_group_id, AgsGroupId group_id, AgsGroupId child_group_id,
									guint audio_signal_level);
  auto void ags_channel_recursive_play_init_arrange_group_id_down(AgsChannel *output,
								  AgsGroupId parent_group_id, AgsGroupId group_id, AgsGroupId child_group_id,
								  guint audio_signal_level);

  auto void ags_channel_recursive_play_init_duplicate_up(AgsChannel *channel,
							 gboolean playback, gboolean sequencer, gboolean notation,
							 AgsGroupId group_id);
  auto void ags_channel_recursive_play_init_duplicate_down_input(AgsChannel *output,
								 gboolean playback, gboolean sequencer, gboolean notation,
								 AgsGroupId group_id,
								 guint audio_signal_level);
  auto void ags_channel_recursive_play_init_duplicate_down(AgsChannel *output,
							   gboolean playback, gboolean sequencer, gboolean notation,
							   AgsGroupId group_id,
							   guint audio_signal_level);

  auto void ags_channel_recursive_play_init_resolve_up(AgsChannel *channel,
						       AgsGroupId group_id,
						       guint audio_signal_level);
  auto void ags_channel_recursive_play_init_resolve_down_input(AgsChannel *output,
							       AgsGroupId group_id,
							   guint audio_signal_level);
  auto void ags_channel_recursive_play_init_resolve_down(AgsChannel *output,
							 AgsGroupId group_id,
							 guint audio_signal_level);

  auto void ags_channel_recursive_play_init_up(AgsChannel *channel);
  auto void ags_channel_recursive_play_init_down_input(AgsChannel *output,
						       AgsGroupId group_id);
  auto void ags_channel_recursive_play_init_down(AgsChannel *output, AgsGroupId group_id);

  /*
   * arrangeing group ids is done from the axis to the root and then from the axis to the leafs
   */
  void ags_channel_recursive_play_init_arrange_group_id_up(AgsChannel *channel,
							   guint audio_signal_level)
  {
    AgsAudio *audio;
    AgsChannel *current;
    AgsRecallID *recall_id;
    
    audio = AGS_AUDIO(channel->audio);
    
    current = channel;
    
    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_recursive_play_init_up0;
    }

    /* goto toplevel AgsChannel */
    while(current != NULL){
      /* AgsInput */
      current->recall_id = ags_recall_id_add(current->recall_id,
					     0, group_id, child_group_id,
					     current->first_recycling, current->last_recycling,
					     ((audio_signal_level > 1) ? TRUE: FALSE));

      /* AgsAudio */
      audio->recall_id = ags_recall_id_add(audio->recall_id,
					   0, group_id, child_group_id,
					   current->first_recycling, current->last_recycling,
					   ((audio_signal_level > 1) ? TRUE: FALSE));

      /* AgsOutput */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output, current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output, current->line);
      }

    ags_channel_recursive_play_init_up0:
      current->recall_id = ags_recall_id_add(current->recall_id,
					     0, group_id, child_group_id,
					     current->first_recycling, current->last_recycling,
					     ((audio_signal_level > 1) ? TRUE: FALSE));

      /* iterate */
      if(current->link == NULL){
	break;
      }
    
      audio = AGS_AUDIO(current->link->audio);
      
      //NOTE:JK: see documentation
      //      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      //	break;
      
      current = current->link;
    }
  }
  void ags_channel_recursive_play_init_arrange_group_id_down_input(AgsChannel *output,
								   AgsGroupId parent_group_id, AgsGroupId group_id, AgsGroupId child_group_id,
								   guint audio_signal_level)
  {
    AgsAudio *audio;
    AgsChannel *input;
    AgsGroupId next_parent_group_id, next_group_id, next_child_group_id;
    guint next_audio_signal_level;

    audio = AGS_AUDIO(output->audio);

    if(audio->input == NULL){
      return;
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      input = ags_channel_nth(audio->input, output->audio_channel);

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	next_parent_group_id = group_id;
	next_group_id = child_group_id;

	next_child_group_id = ags_recall_id_generate_group_id();

	next_audio_signal_level = audio_signal_level + 1;
      }else{
	next_parent_group_id = parent_group_id;
	next_group_id = group_id;
	next_child_group_id = child_group_id;

	next_audio_signal_level = audio_signal_level;
      }
    
      while(input != NULL){
	input->recall_id = ags_recall_id_add(input->recall_id,
					     parent_group_id, group_id, child_group_id,
					     input->first_recycling, input->last_recycling,
					     ((audio_signal_level > 1) ? TRUE: FALSE));

	if(input->link != NULL){
	  ags_channel_recursive_play_init_arrange_group_id_down(input->link,
								next_parent_group_id, next_group_id, next_child_group_id,
								next_audio_signal_level);
	}

	input = input->next_pad;
      }
    }else{
      input = ags_channel_nth(audio->input, output->line);

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	next_parent_group_id = group_id;
	next_group_id = child_group_id;

	next_child_group_id = ags_recall_id_generate_group_id();

	next_audio_signal_level = audio_signal_level + 1;
      }else{
	next_parent_group_id = parent_group_id;
	next_group_id = group_id;
	next_child_group_id = child_group_id;

	next_audio_signal_level = audio_signal_level;
      }

      input->recall_id = ags_recall_id_add(input->recall_id,
					   parent_group_id, group_id, child_group_id,
					   input->first_recycling, input->last_recycling,
					   ((audio_signal_level > 1) ? TRUE: FALSE));

      if(input->link != NULL){
	ags_channel_recursive_play_init_arrange_group_id_down(input->link,
							      next_parent_group_id, next_group_id, next_child_group_id,
							      next_audio_signal_level);
      }
    }
  }
  void ags_channel_recursive_play_init_arrange_group_id_down(AgsChannel *output,
							     AgsGroupId parent_group_id, AgsGroupId group_id, AgsGroupId child_group_id,
							     guint audio_signal_level)
  {
    AgsAudio *audio;

    if(output == NULL){
      return;
    }

    /* AgsOutput */
    output->recall_id = ags_recall_id_add(output->recall_id,
					  parent_group_id, group_id, child_group_id,
					  output->first_recycling, output->last_recycling,
					  ((audio_signal_level > 1) ? TRUE: FALSE));

    /* AgsAudio */
    audio = AGS_AUDIO(output->audio);
        
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      AgsGroupId next_group_id;

      /* generate new group id */
      next_group_id = ags_recall_id_generate_group_id();

      /* add group id to AgsAudio */
      audio->recall_id = ags_recall_id_add(audio->recall_id,
					   parent_group_id, group_id, child_group_id,
					   output->first_recycling, output->last_recycling,
					   ((audio_signal_level > 1) ? TRUE: FALSE));
      
      /* add the new group id to AgsAudio */
      audio->recall_id = ags_recall_id_add(audio->recall_id,
					   group_id, child_group_id, next_group_id,
					   ags_output_find_first_input_recycling(AGS_OUTPUT(output)), ags_output_find_last_input_recycling(AGS_OUTPUT(output)),
					   ((audio_signal_level > 1) ? TRUE: FALSE));
      
      
      /* AgsInput */
      ags_channel_recursive_play_init_arrange_group_id_down_input(output,
								  group_id, child_group_id, next_group_id,
								  audio_signal_level + 1);
    }else{
      /* add group id to AgsAudio */
      audio->recall_id = ags_recall_id_add(audio->recall_id,
					   parent_group_id, group_id, child_group_id,
					   output->first_recycling, output->last_recycling,
					   ((audio_signal_level > 0) ? TRUE: FALSE));

      /* AgsInput */
      ags_channel_recursive_play_init_arrange_group_id_down_input(output,
								  parent_group_id, group_id, child_group_id,
								  audio_signal_level);
    }
  }

  /*
   * duplicateing template recalls is done from the root to the leafes
   *
   * externalized 
   * externalized 
   */

  void ags_channel_recursive_play_init_duplicate_up(AgsChannel *channel,
						    gboolean playback, gboolean sequencer, gboolean notation,
						    AgsGroupId group_id)
  {
    AgsAudio *audio;
    AgsChannel *current;
    AgsRecallID *recall_id;

    if(channel == NULL){
      return;
    }
    
    audio = AGS_AUDIO(channel->audio);

    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_recursive_play_init_duplicate_up0;
    }

    /* goto toplevel AgsChannel */
    while(current != NULL){
      /* AgsInput */
      ags_channel_duplicate_recall(current,
				   playback, sequencer, notation,
				   group_id,
				   0);      

      /* AgsAudio */
      ags_audio_duplicate_recall(audio,
				 playback, sequencer, notation,
				 current->first_recycling, current->last_recycling,
				 group_id,
				 0, TRUE);

      /* AgsOutput */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output, current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output, current->line);
      }

    ags_channel_recursive_play_init_duplicate_up0:
      ags_channel_duplicate_recall(current,
				   playback, sequencer, notation,
				   group_id,
				   0);

      /* iterate */      
      if(current->link == NULL){
	break;
      }
    
      audio = AGS_AUDIO(current->link->audio);
      
      //NOTE:JK: see documentation
      //      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      //	break;
      
      current = current->link;
    }
  }
  void ags_channel_recursive_play_init_duplicate_down_input(AgsChannel *output,
							    gboolean playback, gboolean sequencer, gboolean notation,
							    AgsGroupId group_id,
							    guint audio_signal_level)
  {
    AgsAudio *audio;
    AgsRecycling *first_recycling;
    AgsRecycling *last_recycling;
    AgsChannel *input_start, *input;
    AgsGroupId next_group_id;
    guint next_audio_signal_level;

    audio = AGS_AUDIO(output->audio);

    if(audio->input == NULL){
      return;
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){ /* async order of channels within audio */
      /* retrieve input */
      input_start =
	input = ags_channel_nth(audio->input, output->audio_channel);
    
      /* retrieve group id and audio signal level of link */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	next_group_id = ags_recall_id_find_group_id(input->recall_id, group_id)->child_group_id;
	next_audio_signal_level = audio_signal_level + 1;
      }else{
	next_group_id = group_id;
	next_audio_signal_level = audio_signal_level;
      }
      
      /* duplicate recalls on input */
      while(input != NULL){
	/* duplicate input */
	ags_channel_duplicate_recall(input,
				     playback, sequencer, notation,
				     group_id,
				     audio_signal_level);

	/* iterate */
	input = input->next_pad;
      }

      /* traverse the tree */
      input = input_start;

      while(input != NULL){
	if(input->link != NULL){
	  /* follow the links */
	  ags_channel_recursive_play_init_duplicate_down(input->link,
							 playback, sequencer, notation,
							 next_group_id,
							 next_audio_signal_level);
	}

	/* iterate */
	input = input->next_pad;
      }

    }else{ /* sync order of channels within audio */
      /* retrieve input */
      input = ags_channel_nth(audio->input, output->line);

      /* retrieve group id and audio signal level of link */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	next_group_id = ags_recall_id_find_group_id(input->recall_id, group_id)->child_group_id;
	next_audio_signal_level = audio_signal_level + 1;
      }else{
	next_group_id = child_group_id;
	next_audio_signal_level = audio_signal_level;
      }

      /* duplicate recalls on input */
      ags_channel_duplicate_recall(input,
				   playback, sequencer, notation,
				   group_id,
				   audio_signal_level);

      /* traverse the tree */
      if(input->link != NULL){
	/* follow the links */
	ags_channel_recursive_play_init_duplicate_down(input->link,
						       next_group_id,
						       playback, sequencer, notation,
						       next_audio_signal_level);
      }
    }
  }
  void ags_channel_recursive_play_init_duplicate_down(AgsChannel *output,
						      gboolean playback, gboolean sequencer, gboolean notation,
						      AgsGroupId group_id,
						      guint audio_signal_level)
  {
    AgsAudio *audio;
    AgsRecycling *first_recycling;
    AgsRecycling *last_recycling;
    guint next_audio_signal_level, next_group_id;

    if(output == NULL){
      return;
    }

    /* AgsAudio */
    audio = AGS_AUDIO(output->audio);

    /* duplicate output */
    ags_channel_duplicate_recall(output,
				 playback, sequencer, notation,
				 group_id,
				 audio_signal_level);

    /* retrieve next audio signal level */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      next_audio_signal_level = audio_signal_level + 1;
    }else{
      next_audio_signal_level = audio_signal_level;
    }

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      AgsRecycling *first_recycling, *last_recycling;
      AgsGroupId child_group_id;

      next_group_id = ags_recall_id_find_group_id(output->recall_id, group_id)->child_group_id;
      child_group_id = next_group_id;

      /* duplicate audio */
      ags_audio_duplicate_recall(audio,
				 playback, sequencer, notation,
				 output->first_recycling, output->last_recycling,
				 group_id,
				 next_audio_signal_level, TRUE);

      first_recycling = ags_output_find_first_input_recycling(AGS_OUTPUT(output));
      last_recycling = ags_output_find_last_input_recycling(AGS_OUTPUT(output));

      ags_audio_duplicate_recall(audio,
				 playback, sequencer, notation,
				 first_recycling, last_recycling,
				 next_group_id,
				 next_audio_signal_level, FALSE);

      /* call function which duplicates input */
      ags_channel_recursive_play_init_duplicate_down_input(output,
							   playback, sequencer, notation,
							   child_group_id,
							   next_audio_signal_level);
    }else{
      /* duplicate audio */
      ags_audio_duplicate_recall(audio,
				 playback, sequencer, notation,
				 output->first_recycling, output->last_recycling,
				 group_id,
				 next_audio_signal_level, TRUE);

      /* call function which duplicates input */
      ags_channel_recursive_play_init_duplicate_down_input(output,
							   playback, sequencer, notation,
							   group_id,
							   next_audio_signal_level);
    }
  }

  /*
   * resolving recall dependencies has to be done from the root to the leafs
   */
  void ags_channel_recursive_play_init_resolve_up(AgsChannel *channel,
						  AgsGroupId group_id,
						  guint audio_signal_level)
  {
    AgsAudio *audio;
    AgsChannel *current;
    AgsRecallID *recall_id;
    AgsRecycling *first_recycling, *last_recycling;
    
    if(channel == NULL){
      return;
    }

    audio = AGS_AUDIO(channel->audio);

    first_recycling = channel->first_recycling;
    last_recycling = channel->last_recycling;

    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_recursive_play_init_resolve_up0;
    }

    while(current != NULL){
      /* resolve input */
      ags_channel_resolve_recall(current,
				 group_id);
      
      /* resolve audio */
      ags_audio_resolve_recall(audio,
			       first_recycling, last_recycling,
			       group_id);

      /* retrieve output */
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output,
				  current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output,
				  current->line);
      }

      /* stop if output has recycling */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	return;
      }

      /* resolve output */
    ags_channel_recursive_play_init_resolve_up0:
      ags_channel_resolve_recall(current,
				 group_id);


      /* iterate */      
      if(current->link == NULL)
	break;
    
      audio = AGS_AUDIO(current->link->audio);
      
      //NOTE:JK: see documentation
      //      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      //	break;
      
      current = current->link;
    }
  }
  void ags_channel_recursive_play_init_resolve_down_input(AgsChannel *output,
							  AgsGroupId group_id,
							  guint audio_signal_level)
  {
    AgsAudio *audio;
    AgsChannel *input, *input_start;
    AgsGroupId next_group_id;
    guint next_audio_signal_level;

    audio = AGS_AUDIO(output->audio);

    if(audio->input == NULL){
      return;
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){ /* async order of channels within audio */
      /* retrieve input */
      input_start =
	input = ags_channel_nth(audio->input, output->audio_channel);
    
      /* retrieve group id and audio signal level of link */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	next_group_id = ags_recall_id_find_group_id(input->recall_id, group_id)->child_group_id;
	next_audio_signal_level = audio_signal_level + 1;
      }else{
	next_group_id = group_id;
	next_audio_signal_level = audio_signal_level;
      }

      /* resolve recall dependencies on input */
      while(input != NULL){
	/* resolve input */
	ags_channel_resolve_recall(input,
				   group_id);

	/* iterate */
	input = input->next_pad;
      }

      /* traverse the tree */
      input = input_start;

      while(input != NULL){
	if(input->link != NULL){
	  /* follow the links */
	  ags_channel_recursive_play_init_resolve_down(input->link,
						       next_group_id,
						       next_audio_signal_level);
	}

	/* iterate */
	input = input->next_pad;
      }
    }else{ /* sync order of channels within audio */
      /* retrieve input */
      input = ags_channel_nth(audio->input, output->line);

      /* retrieve group id and audio signal level of link */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	next_group_id = ags_recall_id_find_group_id(input->recall_id, group_id)->child_group_id;
	next_audio_signal_level = audio_signal_level + 1;
      }else{
	next_group_id = child_group_id;
	next_audio_signal_level = audio_signal_level;
      }

      /* resolve recall dependencies on input */
      ags_channel_resolve_recall(input,
				 group_id);

      /* traverse the tree */
      if(input->link != NULL){
	/* follow the links */
	ags_channel_recursive_play_init_resolve_down(input->link,
						     next_group_id,
						     next_audio_signal_level);
      }
    }
  }
  void ags_channel_recursive_play_init_resolve_down(AgsChannel *output,
						    AgsGroupId group_id,
						    guint audio_signal_level)
  {
    AgsAudio *audio;

    if(output == NULL){
      return;
    }

    /* AgsAudio */
    audio = AGS_AUDIO(output->audio);

    /* resolve dependencies on output */
    ags_channel_resolve_recall(output,
			       group_id);
    
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      AgsRecycling *first_recycling, *last_recycling;
      AgsGroupId next_group_id;

      next_group_id = ags_recall_id_find_group_id(output->recall_id, group_id)->child_group_id;

      first_recycling = ags_output_find_first_input_recycling(AGS_OUTPUT(output));
      last_recycling = ags_output_find_last_input_recycling(AGS_OUTPUT(output));

      /* resolve dependencies on audio */
      ags_audio_resolve_recall(audio,
			       output->first_recycling, output->last_recycling,
			       group_id);
      
      ags_audio_resolve_recall(audio,
			       first_recycling, last_recycling,
			       next_group_id);

      /* traverse the tree */
      ags_channel_recursive_play_init_resolve_down_input(output,
							 next_group_id,
							 audio_signal_level + 1);
    }else{
      /* resolve dependencies on audio */
      ags_audio_resolve_recall(audio,
			       output->first_recycling, output->last_recycling,
			       group_id);

      /* traverse the tree */
      ags_channel_recursive_play_init_resolve_down_input(output,
							 group_id,
							 audio_signal_level);
    }
  }

  /*
   * run-initializeing recalls is done from the leafs to the root
   */
  void ags_channel_recursive_play_init_up(AgsChannel *channel)
  {
    AgsAudio *audio;
    AgsChannel *current;
    AgsRecallID *recall_id;

    if(channel == NULL){
      return;
    }

    current = channel;
    
    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_recursive_play_init_up0;
    }else{
      audio = AGS_AUDIO(current->audio);
    }

    /* goto toplevel AgsChannel */
    while(current != NULL){
      //NOTE:JK: see documentation
      //      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      //	break;

      /* AgsInput */
      ags_channel_init_recall(current, stage,
			      group_id);
      
      /* AgsAudio */
      ags_audio_init_recall(audio, stage,
			    first_recycling, last_recycling,
			    group_id);

      /* AgsOutput */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output, current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output, current->line);
      }

    ags_channel_recursive_play_init_up0:
      ags_channel_init_recall(current, stage,
			      group_id);

      /* iterate */      
      if(current->link == NULL){
	break;
      }
    
      audio = AGS_AUDIO(current->link->audio);
            
      current = current->link;
    }
  }
  void ags_channel_recursive_play_init_down_input(AgsChannel *output,
						  AgsGroupId group_id)
  {
    AgsAudio *audio;
    AgsChannel *input;
    AgsGroupId next_group_id;

    audio = AGS_AUDIO(output->audio);

    if(audio->input == NULL){
      return;
    }

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

	ags_channel_init_recall(input, stage,
				group_id);

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

      ags_channel_init_recall(input, stage,
			      group_id);
    }
  }
  void ags_channel_recursive_play_init_down(AgsChannel *output,
					    AgsGroupId group_id)
  {
    AgsAudio *audio;

    if(output == NULL){
      return;
    }

    /* AgsAudio */
    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      AgsRecycling *first_recycling, *last_recycling;
      AgsGroupId next_group_id;

      next_group_id = ags_recall_id_find_group_id(output->recall_id, group_id)->child_group_id;

      ags_audio_init_recall(audio, stage,
			    output->first_recycling, output->last_recycling,
			    group_id);

      first_recycling = ags_output_find_first_input_recycling(AGS_OUTPUT(output));
      last_recycling = ags_output_find_last_input_recycling(AGS_OUTPUT(output));

      ags_audio_init_recall(audio, stage,
			    first_recycling, last_recycling,
			    next_group_id);
      /* AgsInput */
      ags_channel_recursive_play_init_down_input(output,
						 next_group_id);
    }else{
      ags_audio_init_recall(audio, stage,
			    output->first_recycling, output->last_recycling,
			    group_id);

      /* AgsInput */
      ags_channel_recursive_play_init_down_input(output,
						 group_id);
    }

    ags_channel_init_recall(output, stage,
			    group_id);
  }


  /*
   * entry point
   */
  if(channel == NULL){
    return;
  }

  first_recycling = channel->first_recycling;
  last_recycling = channel->last_recycling;

  audio = AGS_AUDIO(channel->audio);

  /* arrange group_id */
  if(arrange_group_id){
    ags_channel_recursive_play_init_arrange_group_id_up(channel,
							audio_signal_level);

    if(AGS_IS_OUTPUT(channel)){
      audio->recall_id = ags_recall_id_add(audio->recall_id,
					   0, group_id, child_group_id,
					   first_recycling, last_recycling,
					   ((audio_signal_level > 1) ? TRUE: FALSE));

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	AgsRecycling *first_recycling;
	AgsRecycling *last_recycling;
	AgsGroupId next_group_id;

	next_group_id = ags_recall_id_generate_group_id();
	
	first_recycling = ags_output_find_first_input_recycling(AGS_OUTPUT(channel));
	last_recycling = ags_output_find_last_input_recycling(AGS_OUTPUT(channel));

	audio->recall_id = ags_recall_id_add(audio->recall_id,
					     group_id, child_group_id, next_group_id,
					     first_recycling, last_recycling,
					     ((audio_signal_level + 1 > 1) ? TRUE: FALSE));

	ags_channel_recursive_play_init_arrange_group_id_down_input(channel,
								    group_id, child_group_id, next_group_id,
								    audio_signal_level + 1);
      }else{
	ags_channel_recursive_play_init_arrange_group_id_down_input(channel,
								    0, group_id, child_group_id,
								    audio_signal_level);
      }
    }else{
      if(channel->link != NULL){
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  ags_channel_recursive_play_init_arrange_group_id_down(channel->link,
								group_id, child_group_id, ags_recall_id_generate_group_id(),
								audio_signal_level + 1);
	}else{
	  ags_channel_recursive_play_init_arrange_group_id_down(channel->link,
								0, group_id, child_group_id,
								audio_signal_level);
	}
      }
    }
  }

  /* duplicate AgsRecall templates */
  if(duplicate_templates){
    AgsGroupId next_group_id;
    guint next_audio_signal_level;

    ags_channel_recursive_play_init_duplicate_up(channel,
						 playback, sequencer, notation,
						 group_id);

    if(AGS_IS_OUTPUT(channel)){
      ags_audio_duplicate_recall(audio,
				 playback, sequencer, notation,
				 first_recycling, last_recycling,
				 group_id,
				 audio_signal_level, TRUE);

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	next_group_id = child_group_id;
	next_audio_signal_level = audio_signal_level + 1;
      }else{
	next_group_id = group_id;
	next_audio_signal_level = audio_signal_level;
      }

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	AgsRecycling *first_recycling, *last_recycling;

	first_recycling = ags_output_find_first_input_recycling(AGS_OUTPUT(channel));
	last_recycling = ags_output_find_last_input_recycling(AGS_OUTPUT(channel));

	ags_audio_duplicate_recall(audio,
				   playback, sequencer, notation,
				   first_recycling, last_recycling,
				   next_group_id,
				   next_audio_signal_level, FALSE);

	ags_channel_recursive_play_init_duplicate_down_input(channel,
							     playback, sequencer, notation,
							     next_group_id,
							     next_audio_signal_level);
      }else{
	ags_channel_recursive_play_init_duplicate_down_input(channel,
							     playback, sequencer, notation,
							     next_group_id,
							     next_audio_signal_level);
      }
    }else{
      /* traverse the tree */
      if(channel->link != NULL){
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  ags_channel_recursive_play_init_duplicate_down(channel->link,
							 playback, sequencer, notation,
							 child_group_id,
							 audio_signal_level + 1);
	}else{
	  ags_channel_recursive_play_init_duplicate_down(channel->link,
							 playback, sequencer, notation,
							 group_id,
							 audio_signal_level);
	}
      }
    }
  }

  /* resolve dependencies */
  if(resolve_dependencies){
    ags_channel_recursive_play_init_resolve_up(channel,
					       group_id,
					       audio_signal_level);

    if(AGS_IS_OUTPUT(channel)){
      ags_audio_resolve_recall(audio,
			       first_recycling, last_recycling,
			       group_id);

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	AgsRecycling *first_recycling;
	AgsRecycling *last_recycling;

	first_recycling = ags_output_find_first_input_recycling(AGS_OUTPUT(channel));
	last_recycling = ags_output_find_last_input_recycling(AGS_OUTPUT(channel));

	ags_audio_resolve_recall(audio,
				 first_recycling, last_recycling,
				 child_group_id);

	ags_channel_recursive_play_init_resolve_down_input(channel,
							   child_group_id,
							   audio_signal_level + 1);
      }else{
	ags_channel_recursive_play_init_resolve_down_input(channel,
							   group_id,
							   audio_signal_level);
      }
    }else{
      /* traverse the tree */
      if(channel->link != NULL){
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  ags_channel_recursive_play_init_resolve_down(channel->link,
						       child_group_id,
						       audio_signal_level + 1);
	}else{
	  ags_channel_recursive_play_init_resolve_down(channel->link,
						       group_id,
						       audio_signal_level);
	}
      }
    }
  }

  /* do the different stages of recall initializiation */
  if(stage == -1){
    stage = 0;
    stage_stop = 3;
  }else{
    stage_stop = stage + 1;
  }

  for(; stage < stage_stop; stage++){
    if(AGS_IS_OUTPUT(channel)){
      ags_audio_init_recall(audio, stage,
			    first_recycling, last_recycling,
			    group_id);

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	AgsRecycling *first_recycling;
	AgsRecycling *last_recycling;

	ags_channel_recursive_play_init_down_input(channel,
						   child_group_id);

	first_recycling = ags_output_find_first_input_recycling(AGS_OUTPUT(channel));
	last_recycling = ags_output_find_last_input_recycling(AGS_OUTPUT(channel));

	ags_audio_init_recall(audio, stage,
			      first_recycling, last_recycling,
			      child_group_id);
      }else{
	ags_channel_recursive_play_init_down_input(channel,
						   group_id);
      }
    }else{
      if(channel->link != NULL){
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  ags_channel_recursive_play_init_down(channel->link,
					       child_group_id);
	}else{
	  ags_channel_recursive_play_init_down(channel->link,
					       group_id);
	}
      }
    }

    ags_channel_recursive_play_init_up(channel);
  }
}

/**
 * ags_channel_cancel:
 * @channel an #AgsChannel
 * @recall_id and #AgsRecallID
 * @do_recall a #gboolean
 *
 * Calls for every matching @recall_id ags_recall_cancel() whereby @do_recall tells what
 * #AgsRecall list should be used from @channel.
 */
void
ags_channel_cancel(AgsChannel *channel, AgsRecallID *recall_id, gboolean do_recall)
{
  AgsRecall *recall;
  GList *list, *list_next;
  AgsGroupId group_id;
  
  if(recall_id != NULL){
    return;
  }

  if(do_recall)
    list = channel->recall;
  else
    list = channel->play;
  
  while(list != NULL){
    list_next = list->next;

    recall = AGS_RECALL(list->data);

    group_id = recall_id->group_id;

    if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0 ||
       recall->recall_id->group_id != group_id){
      list = list_next;
      continue;
    }

    ags_recall_cancel(recall);
    
    list = list_next;
  }
}

/**
 * ags_channel_recursive_cancel:
 * @channel an #AgsChannel
 * @group_id an #AgsGroupId
 *
 * Traverses the tree down and up and calls ags_channel_cancel() for corresponding
 * @group_id.
 */
void
ags_channel_recursive_cancel(AgsChannel *channel, AgsGroupId group_id)
{
  AgsRecallID *recall_id;
  auto void ags_channel_recursive_cancel_output(AgsChannel *output, AgsRecallID *output_recall_id, gboolean do_recall);
  void ags_channel_recursive_cancel_input_sync(AgsChannel *input, AgsRecallID *input_recall_id,
					       gboolean do_recall){
    AgsAudio *audio;
    AgsRecallID *child_recall_id;
    gboolean child_do_recall;

    audio = AGS_AUDIO(input->audio);

    /* check if we go down */
    if(input->link != NULL){
      /* check if there is a new group_id */
      ags_audio_find_group_id_from_child(audio,
					 input, input_recall_id, do_recall,
					 &child_recall_id, &child_do_recall);
      
      /* go down */
      ags_channel_recursive_cancel_output(input->link, child_recall_id,
					  child_do_recall);
    }
      
    /* call input */
    ags_channel_cancel(input, input_recall_id, do_recall);
  }
  void ags_channel_recursive_cancel_input_async(AgsChannel *input,
						AgsGroupId output_group_id,
						gboolean do_recall, gboolean input_has_new_group_id){
    AgsRecallID *input_recall_id;
    
    /* iterate AgsInputs */
    while(input != NULL){
      /* get AgsRecallID of AgsInput */
      ags_channel_find_input_recall_id(input,
				       output_group_id,
				       input_has_new_group_id,
				       &input_recall_id);
	
      ags_channel_recursive_cancel_input_sync(input, input_recall_id,
					      do_recall);
      
      input = input->next_pad;
    }
  }
  void ags_channel_recursive_cancel_output(AgsChannel *output, AgsRecallID *output_recall_id,
					   gboolean do_recall){
    AgsAudio *audio;
    AgsChannel *current, *input;
    AgsRecallID *input_recall_id;
    gboolean input_do_recall, input_has_new_group_id;
    
    audio = AGS_AUDIO(output->audio);
    
    /* call audio */
    ags_audio_cancel(audio,
		     output_recall_id->group_id,
		     output->first_recycling, output->last_recycling,
		     do_recall);
    
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
	if((AGS_RECALL_ID_CANCELED & (output_recall_id->flags)) != 0){
	  /* block sync|async for this run */
	  ags_channel_recall_id_set(output, output_recall_id->group_id, TRUE,
				    AGS_CHANNEL_RECALL_ID_CANCEL,
				    NULL);

	  /* run in AGS_AUDIO_ASYNC mode */
	  ags_channel_recursive_cancel_input_async(ags_channel_nth(audio->input, output->audio_channel),
						   output_recall_id->group_id,
						   input_do_recall, input_has_new_group_id);
	}
      }else{
	AgsChannel *input;
	
	input = ags_channel_nth(audio->input, output->line);
	
	ags_channel_find_input_recall_id(input,
					 output_recall_id->group_id,
					 input_has_new_group_id,
					 &input_recall_id);
	  
	ags_channel_recursive_cancel_input_sync(input, input_recall_id,
						input_do_recall);
	
      }
    }else{
      /* run in AGS_AUDIO_ASYNC mode */
      ags_channel_recursive_cancel_input_async(ags_channel_nth(audio->input, output->audio_channel),
					       output_recall_id->group_id,
					       input_do_recall, input_has_new_group_id);
    }
    
    /* call output */
    ags_channel_cancel(output, output_recall_id, do_recall);
  }
  void ags_channel_recursive_cancel_up(AgsChannel *channel, AgsRecallID *recall_id){
    AgsAudio *audio;

    if(channel == NULL)
      return;

    audio = AGS_AUDIO(channel->audio);
    
    if(AGS_IS_INPUT(channel)){
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;

      goto ags_channel_recursive_cancel_up0;
    }else{
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;
      
      goto ags_channel_recursive_cancel_up1;
    }
    
    while(channel != NULL){
      audio = AGS_AUDIO(channel->audio);

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;

      /* cancel input */
    ags_channel_recursive_cancel_up0:
      ags_channel_cancel(channel, recall_id, FALSE);
      
      /* cancel audio */
      ags_audio_cancel(audio,
		       recall_id->group_id,
		       channel->first_recycling, channel->last_recycling, 
		       FALSE);

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;

      /* get output */
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0)
	channel = ags_channel_nth(audio->output, channel->audio_channel);
      else
	channel = ags_channel_nth(audio->output, channel->line);

      /* cancel output */
    ags_channel_recursive_cancel_up1:
      ags_channel_cancel(channel, recall_id, FALSE);

      channel = channel->link;
    }
  }

  /* entry point */
  recall_id = ags_recall_id_find_group_id(channel->recall_id, group_id);
  
  if(AGS_IS_OUTPUT(channel)){
    ags_channel_recursive_cancel_output(channel, recall_id, FALSE);
  }else{
    ags_channel_recursive_cancel_input_sync(channel, recall_id, FALSE);
  }

  ags_channel_recursive_cancel_up(channel->link, recall_id);
}

/**
 * ags_channel_recursive_reset_group_ids:
 * @channel an #AgsChannel that was linked with @link
 * @link an #AgsChannel that was linked with @channel
 * @old_channel_link the old link of @channel
 * @old_link_link the old link of @link
 *
 * Called by ags_channel_set_link() to handle running #AgsAudio objects correctly.
 * This function destroys #AgsRecall objects which were uneeded because they became
 * invalid due to unlinking. By the way it destroys the uneeded #AgsRecallID objects, too.
 * Additionally it creates #AgsRecall and #AgsRecallID objects to prepare becoming a
 * running object (#AgsAudio or #AgsChannel).
 * By the clean up the invalid #AgsRecall objects will be removed.
 * Once the clean up has done ags_channel_recursive_play_init() will be called for every
 * playing instance that was found.
 */
//TODO:JK: you may want to store querried child id in a tree for optimization
//FIXME:JK: most iterations run from the root to the leafs, but the up going functions doesn't do it correctly and will be called after down going functions, it should hurt but isn't clean
//FIXME:JK: group id for audio has changed
//FIXME:JK: really needs to be revised!
void
ags_channel_recursive_reset_group_ids(AgsChannel *channel, AgsChannel *link,
				      AgsChannel *old_channel_link, AgsChannel *old_link_link)
{
  AgsRecycling *first_recycling, *last_recycling;
  GList *link_upper_devout_play_list, *channel_lower_devout_play_list;
  GList *channel_parent_group_id_list, *channel_group_id_list, *channel_child_group_id_list;
  GList *link_parent_group_id_list, *link_group_id_list, *link_child_group_id_list;
  GList *invalid_upper_group_id_list, *invalid_upper_child_group_id_list;
  GList *invalid_lower_group_id_list, *invalid_lower_child_group_id_list;
  GList *audio_signal_level;
  gint stage;

  /* applying functions */
  auto void ags_audio_reset_group_id(AgsAudio *audio,
				     AgsRecycling *first_recycling, AgsRecycling *last_recycling,
				     GList *parent_group_id_list, GList *group_id_list, GList *child_group_id_list,
				     GList *devout_play_list,
				     GList *audio_signal_level_list, guint audio_signal_level, gboolean output_orientated,
				     GList *invalid_group_id_list);
  auto void ags_channel_reset_group_id(AgsChannel *channel,
				       GList *parent_group_id_list, GList *group_id_list, GList *child_group_id_list,
				       GList *devout_play_list,
				       GList *audio_signal_level_list, guint audio_signal_level,
				       GList *invalid_group_id_list);
  auto void ags_channel_recursive_reset_audio_resolve_recall(AgsAudio *audio,
							     AgsRecycling *first_recycling, AgsRecycling *last_recycling,
							     GList *group_id_list);
  auto void ags_channel_recursive_reset_channel_resolve_recall(AgsChannel *channel,
							       GList *group_id_list);
  auto void ags_channel_recursive_reset_audio_init_recall(AgsAudio *audio,
							  AgsRecycling *first_recycling, AgsRecycling *last_recycling,
							  GList *group_id_list);
  auto void ags_channel_recursive_reset_channel_init_recall(AgsChannel *channel,
							    GList *group_id_list);

  /* utility for lists */
  auto GList* ags_channel_increment_list_with_uint_data(GList *list);
  auto GList* ags_channel_generate_list_with_toplevel_parent_group_id(GList *devout_play_list);
  auto GList* ags_channel_generate_list_with_child_group_id(AgsAudio *audio, AgsChannel *channel,
							    GList *group_id_list, GList *child_group_id_list);
  auto void ags_channel_reset_get_next_group_id_lists(AgsAudio *audio, AgsChannel *channel,
						      gboolean new_level, GList *next_audio_signal_level,
						      GList *parent_group_id_list, GList **next_parent_group_id_list,
						      GList *group_id_list, GList **next_group_id_list,
						      GList *child_group_id_list, GList **next_child_group_id_list,
						      GList *invalid_group_id_list, GList **next_invalid_group_id_list,
						      GList *invalid_child_group_id_list, GList **next_invalid_child_group_id_list);

  /* converters of lists */
  auto GList* ags_channel_devout_play_to_group_id(GList *devout_play_list);
  auto GList* ags_channel_group_id_to_child_group_id(AgsChannel *channel,
						     GList *group_id_list);
  auto GList* ags_channel_find_child_group_id_from_child(AgsChannel *channel,
							 GList *group_id);

  /* collectors of AgsDevoutPlay and group_id */
  auto GList* ags_channel_tillrecycling_collect_devout_play_down_input(AgsChannel *output,
								       GList *list, gboolean collect_group_id);
  auto GList* ags_channel_tillrecycling_collect_devout_play_down(AgsChannel *current,
								 GList *list, gboolean collect_group_id);
  auto GList* ags_channel_recursive_collect_devout_play_up(AgsChannel *channel,
							   gboolean collect_group_id,
							   gboolean retrieve_audio_signal_level, GList **audio_signal_level);

  /* tree iterator functions */
  auto void ags_channel_recursive_reset_group_id_down_input(AgsChannel *output,
							    GList *parent_group_id_list, GList *group_id_list, GList *child_group_id_list,
							    GList *devout_play_list, GList *audio_signal_level,
							    GList *invalid_group_id_list, GList *invalid_child_group_id_list);
  auto void ags_channel_recursive_reset_group_id_down(AgsChannel *current,
						      GList *parent_group_id_list, GList *group_id_list, GList *child_group_id_list,
						      GList *devout_play_list, GList *audio_signal_level,
						      GList *invalid_group_id_list, GList *invalid_child_group_id_list);
  auto void ags_channel_tillrecycling_reset_group_id_up(AgsChannel *channel,
							GList *parent_group_id_list, GList *group_id_list, GList *child_group_id_list,
							GList *devout_play_list,
							GList *invalid_group_id_list);

  auto void ags_channel_recursive_unset_group_id_down_input(AgsChannel *output,
							    GList *invalid_group_id_list, GList *invalid_child_group_id_list);
  auto void ags_channel_recursive_unset_group_id_down(AgsChannel *current,
						      GList *invalid_group_id_list, GList *invalid_child_group_id_list);
  auto void ags_channel_tillrecycling_unset_group_id_up(AgsChannel *channel,
							GList *invalid_group_id_list);

  auto void ags_channel_recursive_resolve_recall_down_input(AgsChannel *channel,
							    GList *group_id, GList *child_group_id);
  auto void ags_channel_recursive_resolve_recall_down(AgsChannel *channel,
						      GList *group_id, GList *child_group_id);
  auto void ags_channel_tillrecycling_resolve_recall_up(AgsChannel *channel,
							GList *group_id);

  auto void ags_channel_recursive_init_recall_down_input(AgsChannel *channel,
							 GList *group_id, GList *child_group_id);
  auto void ags_channel_recursive_init_recall_down(AgsChannel *channel,
						   GList *group_id, GList *child_group_id);
  auto void ags_channel_tillrecycling_init_recall_up(AgsChannel *channel,
						     GList *group_id);

  /* implementation */
  void ags_audio_reset_group_id(AgsAudio *audio,
				AgsRecycling *first_recycling, AgsRecycling *last_recycling,
				GList *parent_group_id_list, GList *group_id_list, GList *child_group_id_list,
				GList *devout_play_list,
				GList *audio_signal_level_list, guint audio_signal_level, gboolean output_orientated,
				GList *invalid_group_id_list){
    AgsRecall *recall;
    AgsRecallID *recall_id;
    AgsDevoutPlay *devout_play;
    AgsGroupId parent_group_id, group_id, child_group_id;
    GList *list, *recall_list;
    guint level;
    gboolean play;
    gboolean playback, sequencer, notation;

    while(invalid_group_id_list != NULL){
      group_id = AGS_POINTER_TO_GROUP_ID(invalid_group_id_list->data);
      recall_id = ags_recall_id_find_group_id_with_recycling(audio->recall_id,
							     group_id,
							     first_recycling, last_recycling);
      
      play = (recall_id->parent_group_id == 0) ? TRUE: FALSE;
      recall_list = play ? audio->play: audio->recall;

      /* remove AgsRecalls */
      while((list = ags_recall_find_group_id(recall_list, group_id)) != NULL){
	recall = AGS_RECALL(list->data);
	
	ags_audio_remove_recall(audio, (GObject *) recall, play);
	
	list = list->next;
      }
      
      /* remove AgsRecallID */
      ags_audio_remove_recall_id(audio, (GObject *) recall_id);

      /* iterate */
      invalid_group_id_list = invalid_group_id_list->next;
    }
    
    while(group_id_list != NULL){
      /* create new AgsRecallIDs */
      parent_group_id = AGS_POINTER_TO_GROUP_ID(parent_group_id_list->data);
      group_id = AGS_POINTER_TO_GROUP_ID(group_id_list->data);
      child_group_id = AGS_POINTER_TO_GROUP_ID(child_group_id_list->data);

      level = (audio_signal_level_list != NULL) ? GPOINTER_TO_UINT(audio_signal_level_list->data): audio_signal_level;

      recall_id = ags_recall_id_new();

      if(parent_group_id == 0){
	audio->recall_id = ags_recall_id_add(audio->recall_id,
					     0, group_id, child_group_id,
					     first_recycling, last_recycling,
					     ((level > 1) ? TRUE: FALSE));
      }else{
	audio->recall_id = ags_recall_id_add(audio->recall_id,
					     parent_group_id, group_id, child_group_id,
					     first_recycling, last_recycling,
					     FALSE);
      }

      /* retrieve the recalls purpose */
      devout_play = AGS_DEVOUT_PLAY(devout_play_list->data);

      playback = ((AGS_DEVOUT_PLAY_PLAYBACK & (devout_play->flags)) != 0) ? TRUE: FALSE;
      sequencer = ((AGS_DEVOUT_PLAY_SEQUENCER & (devout_play->flags)) != 0) ? TRUE: FALSE;
      notation = ((AGS_DEVOUT_PLAY_NOTATION & (devout_play->flags)) != 0) ? TRUE: FALSE;

      /* create new AgsRecalls */
      ags_audio_duplicate_recall(audio,
				 playback, sequencer, notation,
				 first_recycling, last_recycling,
				 group_id,
				 level, output_orientated);

      /* iterate */
      parent_group_id_list = parent_group_id_list->next;
      group_id_list = group_id_list->next;
      child_group_id_list = child_group_id_list->next;
      devout_play_list = devout_play_list->next;

      if(audio_signal_level_list != NULL){
	audio_signal_level_list = audio_signal_level_list->next;
      }
    }
  }
  void ags_channel_reset_group_id(AgsChannel *channel,
				  GList *parent_group_id_list, GList *group_id_list, GList *child_group_id_list,
				  GList *devout_play_list,
				  GList *audio_signal_level_list, guint audio_signal_level,
				  GList *invalid_group_id_list){
    AgsRecall *recall;
    AgsRecallID *recall_id;
    AgsDevoutPlay *devout_play;
    AgsGroupId parent_group_id, group_id, child_group_id;
    GList *list, *recall_list;
    guint level;
    gboolean play;
    gboolean playback, sequencer, notation;

    while(invalid_group_id_list != NULL){
      group_id = AGS_POINTER_TO_GROUP_ID(invalid_group_id_list->data);
      recall_id = ags_recall_id_find_group_id(channel->recall_id, group_id);
      
      play = (recall_id->parent_group_id == 0) ? TRUE: FALSE;
      recall_list = play ? channel->play: channel->recall;

      /* unref AgsRecalls */
      while((list = ags_recall_find_group_id(recall_list, group_id)) != NULL){
	recall = AGS_RECALL(list->data);
	
	ags_channel_remove_recall(channel, (GObject *) recall, play);
	
	list = list->next;
      }
      
      /* unref AgsRecallID */
      ags_channel_remove_recall_id(channel, recall_id);

      /* iterate */
      invalid_group_id_list = invalid_group_id_list->next;
    }
    
    while(group_id_list != NULL){
      /* create new AgsRecallIDs */
      parent_group_id = AGS_POINTER_TO_GROUP_ID(parent_group_id_list->data);
      group_id = AGS_POINTER_TO_GROUP_ID(group_id_list->data);
      child_group_id = AGS_POINTER_TO_GROUP_ID(child_group_id_list->data);

      level = (audio_signal_level_list != NULL) ? GPOINTER_TO_UINT(audio_signal_level_list->data): audio_signal_level;

      recall_id = ags_recall_id_new();
	
      if(parent_group_id == 0){
	channel->recall_id = ags_recall_id_add(channel->recall_id,
					       0, group_id, child_group_id,
					       channel->first_recycling, channel->last_recycling,
					       FALSE);

      }else{
	channel->recall_id = ags_recall_id_add(channel->recall_id,
					       parent_group_id, group_id, child_group_id,
					       channel->first_recycling, channel->last_recycling,
					       ((level > 1) ? TRUE: FALSE));
      }

      /* retrieve the recalls purpose */
      devout_play = AGS_DEVOUT_PLAY(devout_play_list->data);

      playback = ((AGS_DEVOUT_PLAY_PLAYBACK & (devout_play->flags)) != 0) ? TRUE: FALSE;
      sequencer = ((AGS_DEVOUT_PLAY_SEQUENCER & (devout_play->flags)) != 0) ? TRUE: FALSE;
      notation = ((AGS_DEVOUT_PLAY_NOTATION & (devout_play->flags)) != 0) ? TRUE: FALSE;

      /* create new AgsRecalls */
      ags_channel_duplicate_recall(channel,
				   playback, sequencer, notation,
				   group_id,
				   level);

      /* iterate */
      parent_group_id_list = parent_group_id_list->next;
      group_id_list = group_id_list->next;
      child_group_id_list = child_group_id_list->next;
      devout_play_list = devout_play_list->next;

      if(audio_signal_level_list != NULL){
	audio_signal_level_list = audio_signal_level_list->next;
      }
    }
  }
  void ags_channel_recursive_reset_audio_resolve_recall(AgsAudio *audio,
							AgsRecycling *first_recycling, AgsRecycling *last_recycling,
							GList *group_id_list){
    AgsGroupId group_id;

    while(group_id_list != NULL){
      group_id = AGS_POINTER_TO_GROUP_ID(group_id_list->data);

      ags_audio_resolve_recall(audio,
			       first_recycling, last_recycling,
			       group_id);

      group_id_list = group_id_list->next;
    }
  }
  void ags_channel_recursive_reset_channel_resolve_recall(AgsChannel *channel,
							  GList *group_id_list){
    AgsGroupId group_id;

    while(group_id_list != NULL){
      group_id = AGS_POINTER_TO_GROUP_ID(group_id_list->data);

      ags_channel_resolve_recall(channel,
				 group_id);

      group_id_list = group_id_list->next;
    }    
  }
  void ags_channel_recursive_reset_audio_init_recall(AgsAudio *audio,
						     AgsRecycling *first_recycling, AgsRecycling *last_recycling,
						     GList *group_id_list){
    AgsGroupId group_id;

    while(group_id_list != NULL){
      group_id = AGS_POINTER_TO_GROUP_ID(group_id_list->data);

      ags_audio_init_recall(audio, stage,
			    first_recycling, last_recycling,
			    group_id);

      group_id_list = group_id_list->next;
    } 
  }
  void ags_channel_recursive_reset_channel_init_recall(AgsChannel *channel,
						       GList *group_id_list){
    AgsGroupId group_id;

    while(group_id_list != NULL){
      group_id = AGS_POINTER_TO_GROUP_ID(group_id_list->data);

      ags_channel_init_recall(channel, stage,
			      group_id);

      group_id_list = group_id_list->next;
    } 
  }
  GList* ags_channel_devout_play_to_group_id(GList *devout_play_list){
    GList *list;

    list = NULL;

    while(devout_play_list != NULL){
      list = g_list_prepend(list,
			    AGS_GROUP_ID_TO_POINTER(AGS_DEVOUT_PLAY(devout_play_list->data)->group_id));


      devout_play_list = devout_play_list->next;
    }

    list = g_list_reverse(list);

    return(list);
  }
  GList* ags_channel_generate_list_with_child_group_id(AgsAudio *audio, AgsChannel *channel,
						       GList *group_id_list, GList *child_group_id_list){
    AgsRecallID *recall_id;
    GList *next_child_group_id_list;
    AgsGroupId next_parent_group_id, next_group_id, next_child_group_id;

    next_child_group_id_list = NULL;

    while(group_id_list != NULL){
      next_parent_group_id = AGS_POINTER_TO_GROUP_ID(group_id_list->data);
      next_group_id = AGS_POINTER_TO_GROUP_ID(child_group_id_list->data);
      next_child_group_id = ags_recall_id_generate_group_id();

      next_child_group_id_list = g_list_prepend(next_child_group_id_list,
						AGS_GROUP_ID_TO_POINTER(next_child_group_id));

      group_id_list = group_id_list->next;
      child_group_id_list = child_group_id_list->next;
    }

    return(next_child_group_id_list);
  }
  void ags_channel_reset_get_next_group_id_lists(AgsAudio *audio, AgsChannel *channel,
						 gboolean new_level, GList *next_audio_signal_level,
						 GList *parent_group_id_list, GList **next_parent_group_id_list,
						 GList *group_id_list, GList **next_group_id_list,
						 GList *child_group_id_list, GList **next_child_group_id_list,
						 GList *invalid_group_id_list, GList **next_invalid_group_id_list,
						 GList *invalid_child_group_id_list, GList **next_invalid_child_group_id_list){
    if(new_level){
      *next_parent_group_id_list = group_id_list;
      *next_group_id_list = child_group_id_list;

      *next_child_group_id_list = ags_channel_generate_list_with_child_group_id(audio, channel,
										group_id_list, child_group_id_list);

      *next_invalid_group_id_list = invalid_child_group_id_list;
      *next_invalid_child_group_id_list = ags_channel_find_child_group_id_from_child(channel,
										     invalid_child_group_id_list);
    }else{
      *next_parent_group_id_list = parent_group_id_list;
      *next_group_id_list = group_id_list;
      *next_child_group_id_list = child_group_id_list;

      *next_invalid_group_id_list = invalid_group_id_list;
      *next_invalid_child_group_id_list = invalid_child_group_id_list;
    }
  }
  GList* ags_channel_increment_list_with_uint_data(GList *list){
    GList *incremented_list;
    guint data;

    incremented_list = NULL;

    while(list != NULL){
      data = GPOINTER_TO_UINT(list->data);
      data++;
      incremented_list = g_list_prepend(incremented_list,
					GUINT_TO_POINTER(data));

      list = list->next;
    }

    incremented_list = g_list_reverse(incremented_list);

    return(incremented_list);
  }
  GList* ags_channel_generate_list_with_toplevel_parent_group_id(GList *devout_play_list){
    GList *parent_group_id;

    parent_group_id = NULL;

    while(devout_play_list != NULL){
      parent_group_id = g_list_prepend(parent_group_id, GUINT_TO_POINTER(0));

      devout_play_list = devout_play_list->next;
    }

    return(parent_group_id);
  }
  GList* ags_channel_group_id_to_child_group_id(AgsChannel *channel,
						GList *group_id_list){
    AgsRecallID *recall_id;
    GList *new_group_id_list;
    GList *list;
    AgsGroupId group_id;

    new_group_id_list = NULL;

    /* retrieve new list */
    list = group_id_list;

    while(list != NULL){
      group_id = AGS_POINTER_TO_GROUP_ID(list->data);
      recall_id = ags_recall_id_find_group_id(channel->recall_id,
					      group_id);

      new_group_id_list = g_list_prepend(new_group_id_list,
					 AGS_GROUP_ID_TO_POINTER(recall_id->child_group_id));

      list = list->next;
    }

    new_group_id_list = g_list_reverse(new_group_id_list);

    return(new_group_id_list);
  }
  GList* ags_channel_find_child_group_id_from_child(AgsChannel *channel,
						    GList *child_group_id){
    AgsAudio *audio;
    AgsRecallID *recall_id;
    GList *next_child_group_id;
    AgsGroupId group_id;

    if(channel == NULL)
      return(NULL);

    audio = AGS_AUDIO(channel->audio);
    next_child_group_id = NULL;

    if(AGS_IS_OUTPUT(channel)){
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
	return(child_group_id);
      }

      while(child_group_id != NULL){
	group_id = AGS_POINTER_TO_GROUP_ID(child_group_id->data);

	recall_id = ags_recall_id_find_parent_group_id(audio->recall_id,
						       group_id);

	next_child_group_id = g_list_prepend(next_child_group_id,
					     AGS_GROUP_ID_TO_POINTER(recall_id->group_id));

	child_group_id = child_group_id->next;
      }
    }else{
      AgsChannel *child;

      child = channel->link;

      if(child == NULL)
	return(NULL);

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	return(child_group_id);
      }

      while(child_group_id != NULL){
	group_id = AGS_POINTER_TO_GROUP_ID(child_group_id->data);

	recall_id = ags_recall_id_find_parent_group_id(child->recall_id,
						       group_id);

	next_child_group_id = g_list_prepend(next_child_group_id,
					     AGS_GROUP_ID_TO_POINTER(recall_id->group_id));

	child_group_id = child_group_id->next;
      }
    }

    return(next_child_group_id);
  }
  GList* ags_channel_tillrecycling_collect_devout_play_down_input(AgsChannel *output,
								  GList *list, gboolean collect_group_id){
    AgsAudio *audio;
    AgsChannel *current;
    gboolean check_link;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      return(list);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input, output->audio_channel);

      while(current != NULL){
	/* collect group id and the recalls purpose */
	if(current->devout_play != NULL &&
	   ((AGS_DEVOUT_PLAY_PLAYBACK & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 ||
	    (AGS_DEVOUT_PLAY_SEQUENCER & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 ||
	    (AGS_DEVOUT_PLAY_NOTATION & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0)){
	  list = g_list_prepend(list,
				(collect_group_id ? AGS_GROUP_ID_TO_POINTER(AGS_DEVOUT_PLAY(current->devout_play)->group_id): current->devout_play));
	}
	/* go down */
	list = ags_channel_tillrecycling_collect_devout_play_down(current->link,
								  list, collect_group_id);

	current = current->next_pad;
      }

    }else{
      current = ags_channel_nth(audio->input, output->line);

      /* collect group id and the recalls purpose */
      if(current->devout_play != NULL &&
	 ((AGS_DEVOUT_PLAY_PLAYBACK & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 ||
	  (AGS_DEVOUT_PLAY_SEQUENCER & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 ||
	  (AGS_DEVOUT_PLAY_NOTATION & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0)){
	list = g_list_prepend(list,
			      (collect_group_id ? AGS_GROUP_ID_TO_POINTER(AGS_DEVOUT_PLAY(current->devout_play)->group_id): current->devout_play));
      }

      /* go down */
      list = ags_channel_tillrecycling_collect_devout_play_down(current->link,
								list, collect_group_id);
    }

    return(list);
  }
  GList* ags_channel_tillrecycling_collect_devout_play_down(AgsChannel *current,
							    GList *list, gboolean collect_group_id){
    AgsAudio *audio;

    if(current == NULL)
      return(list);

    audio = AGS_AUDIO(current->audio);

    /* collect devout play */
    if(current->devout_play != NULL &&
       ((AGS_DEVOUT_PLAY_PLAYBACK & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 ||
	(AGS_DEVOUT_PLAY_SEQUENCER & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 ||
	(AGS_DEVOUT_PLAY_NOTATION & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0)){
      list = g_list_prepend(list,
			    (collect_group_id ? AGS_GROUP_ID_TO_POINTER(AGS_DEVOUT_PLAY(current->devout_play)->group_id): current->devout_play));
    }

    /* collect devout play */
    if(audio->devout_play != NULL &&
       ((AGS_DEVOUT_PLAY_PLAYBACK & (AGS_DEVOUT_PLAY(audio->devout_play)->flags)) != 0 ||
	(AGS_DEVOUT_PLAY_SEQUENCER & (AGS_DEVOUT_PLAY(audio->devout_play)->flags)) != 0 ||
	(AGS_DEVOUT_PLAY_NOTATION & (AGS_DEVOUT_PLAY(audio->devout_play)->flags)) != 0)){
      list = g_list_prepend(list,
			    (collect_group_id ? AGS_GROUP_ID_TO_POINTER(AGS_DEVOUT_PLAY(audio->devout_play)->group_id): audio->devout_play));
    }
    
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
      return(list);

    /* go down */
    list = ags_channel_tillrecycling_collect_devout_play_down_input(current,
								    list, collect_group_id);

    return(list);
  }
  GList* ags_channel_recursive_collect_devout_play_up(AgsChannel *channel,
						      gboolean collect_group_id,
						      gboolean retrieve_audio_signal_level, GList **audio_signal_level){
    AgsAudio *audio;
    AgsChannel *current;
    GList *list, *level_list;
    guint level;

    if(channel == NULL){
      if(retrieve_audio_signal_level){
	*audio_signal_level = NULL;
      }

      return(NULL);
    }

    current = channel;
    list = NULL;

    if(retrieve_audio_signal_level){
      level_list = NULL;
      level = 0;
    }

    if(AGS_IS_OUTPUT(channel)){
      audio = AGS_AUDIO(current->audio);

      goto ags_channel_recursive_collect_devout_play_upOUTPUT;
    }

    while(current != NULL){
      audio = AGS_AUDIO(current->audio);

      /* input */
      if(retrieve_audio_signal_level &&
	 (AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	level++;
      }

      if(current->devout_play != NULL &&
	 ((AGS_DEVOUT_PLAY_PLAYBACK & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 ||
	  (AGS_DEVOUT_PLAY_SEQUENCER & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 ||
	  (AGS_DEVOUT_PLAY_NOTATION & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0)){
	list = g_list_prepend(list,
			      (collect_group_id ? AGS_GROUP_ID_TO_POINTER(AGS_DEVOUT_PLAY(current->devout_play)->group_id): current->devout_play));

	if(retrieve_audio_signal_level){
	  level_list = g_list_prepend(level_list,
				      GUINT_TO_POINTER(level));
	}
      }

      /* audio */
      if(retrieve_audio_signal_level &&
	 (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	level++;
      }

      if(audio->devout_play != NULL &&
	 ((AGS_DEVOUT_PLAY_PLAYBACK & (AGS_DEVOUT_PLAY(audio->devout_play)->flags)) != 0 ||
	  (AGS_DEVOUT_PLAY_SEQUENCER & (AGS_DEVOUT_PLAY(audio->devout_play)->flags)) != 0 ||
	  (AGS_DEVOUT_PLAY_NOTATION & (AGS_DEVOUT_PLAY(audio->devout_play)->flags)) != 0)){
	list = g_list_prepend(list,
			      (collect_group_id ? AGS_GROUP_ID_TO_POINTER(AGS_DEVOUT_PLAY(audio->devout_play)->group_id): audio->devout_play));

	if(retrieve_audio_signal_level){
	  level_list = g_list_prepend(level_list,
				      GUINT_TO_POINTER(level));
	}
      }

      /* output */
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output,
				  current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output,
				  current->line);
      }

    ags_channel_recursive_collect_devout_play_upOUTPUT:
      if(current->devout_play != NULL &&
	 ((AGS_DEVOUT_PLAY_PLAYBACK & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 ||
	  (AGS_DEVOUT_PLAY_SEQUENCER & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 ||
	  (AGS_DEVOUT_PLAY_NOTATION & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0)){
	list = g_list_prepend(list,
			      (collect_group_id ? AGS_GROUP_ID_TO_POINTER(AGS_DEVOUT_PLAY(current->devout_play)->group_id): current->devout_play));

	if(retrieve_audio_signal_level){
	  level_list = g_list_prepend(level_list,
				      GUINT_TO_POINTER(level));
	}
      }

      current = current->link;
    }

    if(retrieve_audio_signal_level){
      *audio_signal_level = level_list;
    }

    return(list);
  }
  void ags_channel_recursive_reset_group_id_down_input(AgsChannel *output,
						       GList *parent_group_id_list, GList *group_id_list, GList *child_group_id_list,
						       GList *devout_play_list, GList *audio_signal_level,
						       GList *invalid_group_id_list, GList *invalid_child_group_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;
    GList *next_parent_group_id_list, *next_group_id_list, *next_child_group_id_list;
    GList *next_invalid_group_id_list, *next_invalid_child_group_id_list;
    GList *next_audio_signal_level;
    gboolean new_level;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
      next_audio_signal_level = ags_channel_increment_list_with_uint_data(audio_signal_level);
      new_level = TRUE;
    }else{
      next_audio_signal_level = audio_signal_level;
      new_level = FALSE;
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input, output->audio_channel);

      while(current != NULL){
	/* reset AgsInput */
	ags_channel_reset_group_id(current,
				   parent_group_id_list, group_id_list, child_group_id_list,
				   devout_play_list,
				   audio_signal_level, G_MAXUINT,
				   invalid_group_id_list);

	/* retrieve next AgsGroupIds */
	ags_channel_reset_get_next_group_id_lists(audio, current,
						  new_level, next_audio_signal_level,
						  parent_group_id_list, &next_parent_group_id_list,
						  group_id_list, &next_group_id_list,
						  child_group_id_list, &next_child_group_id_list,
						  invalid_group_id_list, &next_invalid_group_id_list,
						  invalid_child_group_id_list, &next_invalid_child_group_id_list);

	/* go down */
	ags_channel_recursive_reset_group_id_down(current->link,
						  next_parent_group_id_list, next_group_id_list, next_child_group_id_list,
						  devout_play_list, next_audio_signal_level,
						  next_invalid_group_id_list, next_invalid_child_group_id_list);

	/* free allocated lists */
	if(new_level){
	  g_list_free(next_child_group_id_list);

	  g_list_free(next_audio_signal_level);

	  g_list_free(next_invalid_child_group_id_list);
	}

	current = current->next_pad;
      }
    }else{
      current = ags_channel_nth(audio->input, output->line);

      /* reset AgsInput */
      ags_channel_reset_group_id(current,
				 parent_group_id_list, group_id_list, child_group_id_list,
				 devout_play_list,
				 audio_signal_level, G_MAXUINT,
				 invalid_group_id_list);

      /* retrieve next AgsGroupIds */
      ags_channel_reset_get_next_group_id_lists(audio, current,
						new_level, next_audio_signal_level,
						parent_group_id_list, &next_parent_group_id_list,
						group_id_list, &next_group_id_list,
						child_group_id_list, &next_child_group_id_list,
						invalid_group_id_list, &next_invalid_group_id_list,
						invalid_child_group_id_list, &next_invalid_child_group_id_list);

      /* go down */
      ags_channel_recursive_reset_group_id_down(current->link,
						next_parent_group_id_list, next_group_id_list, next_child_group_id_list,
						devout_play_list, next_audio_signal_level,
						next_invalid_group_id_list, next_invalid_child_group_id_list);

      /* free allocated lists */
      if(new_level){
	g_list_free(next_child_group_id_list);

	g_list_free(next_audio_signal_level);

	g_list_free(next_invalid_child_group_id_list);
      }
    }
  }
  void ags_channel_recursive_reset_group_id_down(AgsChannel *current,
						 GList *parent_group_id_list, GList *group_id_list, GList *child_group_id_list,
						 GList *devout_play_list, GList *audio_signal_level,
						 GList *invalid_group_id_list, GList *invalid_child_group_id_list)
  {
    AgsAudio *audio;
    GList *next_parent_group_id_list, *next_group_id_list, *next_child_group_id_list;
    GList *next_invalid_group_id_list, *next_invalid_child_group_id_list;
    GList *next_audio_signal_level;
    gboolean new_level;

    if(current == NULL)
      return;

    audio = AGS_AUDIO(current->audio);
    
    /* reset on AgsOutput */
    ags_channel_reset_group_id(current,
			       parent_group_id_list, group_id_list, child_group_id_list,
			       devout_play_list,
			       audio_signal_level, G_MAXUINT,
			       invalid_group_id_list);


    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      next_audio_signal_level = ags_channel_increment_list_with_uint_data(audio_signal_level);
      new_level = TRUE;
    }else{
      next_audio_signal_level = audio_signal_level;
      new_level = FALSE;
    }

    /* reset on AgsAudio */
    ags_audio_reset_group_id(audio,
			     current->first_recycling, current->last_recycling,
			     parent_group_id_list, group_id_list, child_group_id_list,
			     devout_play_list,
			     audio_signal_level, G_MAXUINT, TRUE,
			     invalid_group_id_list);

    /* retrieve next AgsGroupIds */
    ags_channel_reset_get_next_group_id_lists(audio, current,
					      new_level, next_audio_signal_level,
					      parent_group_id_list, &next_parent_group_id_list,
					      group_id_list, &next_group_id_list,
					      child_group_id_list, &next_child_group_id_list,
					      invalid_group_id_list, &next_invalid_group_id_list,
					      invalid_child_group_id_list, &next_invalid_child_group_id_list);

    if(new_level){
      /* reset on AgsAudio */
      ags_audio_reset_group_id(audio,
			       ags_output_find_first_input_recycling(AGS_OUTPUT(current)), ags_output_find_last_input_recycling(AGS_OUTPUT(current)),
			       parent_group_id_list, group_id_list, child_group_id_list,
			       devout_play_list,
			       audio_signal_level, G_MAXUINT, FALSE,
			       invalid_group_id_list);

      /* reset new group id on AgsAudio */
      ags_audio_reset_group_id(audio,
			       ags_output_find_first_input_recycling(AGS_OUTPUT(current)), ags_output_find_last_input_recycling(AGS_OUTPUT(current)),
			       next_parent_group_id_list, next_group_id_list, next_child_group_id_list,
			       devout_play_list,
			       next_audio_signal_level, G_MAXUINT, FALSE,
			       next_invalid_group_id_list);
    }else{
      /* reset on AgsAudio */
      ags_audio_reset_group_id(audio,
			       current->first_recycling, current->last_recycling,
			       parent_group_id_list, group_id_list, child_group_id_list,
			       devout_play_list,
			       audio_signal_level, G_MAXUINT, FALSE,
			       invalid_group_id_list);
    }

    /* go down */
    ags_channel_recursive_reset_group_id_down_input(current,
						    next_parent_group_id_list, next_group_id_list, next_child_group_id_list,
						    devout_play_list, next_audio_signal_level,
						    next_invalid_group_id_list, next_invalid_child_group_id_list);

    /* free allocated lists */
    if(new_level){
      g_list_free(next_child_group_id_list);

      g_list_free(next_audio_signal_level);

      g_list_free(next_invalid_child_group_id_list);
    }
  }
  void ags_channel_tillrecycling_reset_group_id_up(AgsChannel *channel,
						   GList *parent_group_id_list, GList *group_id_list, GList *child_group_id_list,
						   GList *devout_play_list,
						   GList *invalid_group_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL)
      return;

    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      audio = AGS_AUDIO(current->audio);

      goto ags_channel_tillrecycling_reset_group_id_upOUTPUT;
    }
    
    while(current != NULL){
      audio = AGS_AUDIO(current->audio);

      /* input */
      ags_channel_reset_group_id(current,
				 parent_group_id_list, group_id_list, child_group_id_list,
				 devout_play_list,
				 NULL, 0,
				 invalid_group_id_list);


      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }      

      /* audio */
      ags_audio_reset_group_id(audio,
			       first_recycling, last_recycling,
			       parent_group_id_list, group_id_list, child_group_id_list,
			       devout_play_list,
			       NULL, 0, FALSE,
			       invalid_group_id_list);

      /* output */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output,
				  current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output,
				  current->line);
      }

    ags_channel_tillrecycling_reset_group_id_upOUTPUT:
      ags_audio_reset_group_id(audio,
			       first_recycling, last_recycling,
			       parent_group_id_list, group_id_list, child_group_id_list,
			       devout_play_list,
			       NULL, 0, TRUE,
			       invalid_group_id_list);

      ags_channel_reset_group_id(current,
				 parent_group_id_list, group_id_list, child_group_id_list,
				 devout_play_list,
				 NULL, 0,
				 invalid_group_id_list);

      current = current->link;
    }
  }
  void ags_channel_recursive_unset_group_id_down_input(AgsChannel *output,
						       GList *invalid_group_id_list, GList *invalid_child_group_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;
    GList *next_invalid_group_id_list, *next_invalid_child_group_id_list;
    gboolean new_level;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
      new_level = TRUE;
    }else{
      new_level = FALSE;
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input, output->audio_channel);

      while(current != NULL){
	/* reset AgsInput */
	ags_channel_reset_group_id(current,
				   NULL, NULL, NULL,
				   NULL,
				   NULL, G_MAXUINT,
				   invalid_group_id_list);

	/* retrieve next AgsGroupIds */
	if(new_level){
	  next_invalid_group_id_list = invalid_child_group_id_list;
	  next_invalid_child_group_id_list = ags_channel_find_child_group_id_from_child(current,
											invalid_child_group_id_list);
	}else{
	  next_invalid_group_id_list = invalid_group_id_list;
	  next_invalid_child_group_id_list = invalid_child_group_id_list;
	}

	/* go down */
	ags_channel_recursive_unset_group_id_down(current->link,
						  next_invalid_group_id_list, next_invalid_child_group_id_list);

	/* free allocated lists */
	if(new_level){
	  g_list_free(next_invalid_child_group_id_list);
	}

	current = current->next_pad;
      }
    }else{
      current = ags_channel_nth(audio->input, output->line);

      /* reset AgsInput */
      ags_channel_reset_group_id(current,
				 NULL, NULL, NULL,
				 NULL,
				 NULL, G_MAXUINT,
				 invalid_group_id_list);

      /* retrieve next AgsGroupIds */
      if(new_level){
	next_invalid_group_id_list = invalid_child_group_id_list;
	invalid_child_group_id_list = ags_channel_find_child_group_id_from_child(current,
										 invalid_child_group_id_list);
      }else{
	next_invalid_group_id_list = invalid_group_id_list;
	next_invalid_child_group_id_list = invalid_child_group_id_list;
      }
      
      /* go down */
      ags_channel_recursive_unset_group_id_down(current->link,
						next_invalid_group_id_list, next_invalid_child_group_id_list);

      /* free allocated lists */
      if(new_level){
	g_list_free(next_invalid_child_group_id_list);
      }
    }

  }
  void ags_channel_recursive_unset_group_id_down(AgsChannel *current,
						 GList *invalid_group_id_list, GList *invalid_child_group_id_list)
  {
    AgsAudio *audio;
    GList *next_invalid_group_id_list, *next_invalid_child_group_id_list;
    gboolean new_level;

    if(current == NULL)
      return;

    audio = AGS_AUDIO(current->audio);
    
    /* reset on AgsOutput */
    ags_channel_reset_group_id(current,
			       NULL, NULL, NULL,
			       NULL,
			       NULL, G_MAXUINT,
			       invalid_group_id_list);

    /* retrieve next AgsGroupIds */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      next_invalid_group_id_list = invalid_child_group_id_list;
      next_invalid_child_group_id_list = ags_channel_find_child_group_id_from_child(current,
										    invalid_child_group_id_list);

      new_level = TRUE;
    }else{
      next_invalid_group_id_list = invalid_group_id_list;
      next_invalid_child_group_id_list = invalid_child_group_id_list;

      new_level = FALSE;
    }

    /* reset on AgsAudio */
    ags_audio_reset_group_id(audio,
			     current->first_recycling, current->last_recycling,
			     NULL, NULL, NULL,
			     NULL,
			     NULL, G_MAXUINT, TRUE,
			     invalid_group_id_list);

    if(new_level){
      ags_audio_reset_group_id(audio,
			       ags_output_find_first_input_recycling(AGS_OUTPUT(current)), ags_output_find_last_input_recycling(AGS_OUTPUT(current)),
			       NULL, NULL, NULL,
			       NULL,
			       NULL, G_MAXUINT, FALSE,
			       invalid_group_id_list);

      ags_audio_reset_group_id(audio,
			       ags_output_find_first_input_recycling(AGS_OUTPUT(current)), ags_output_find_last_input_recycling(AGS_OUTPUT(current)),
			       NULL, NULL, NULL,
			       NULL,
			       NULL, G_MAXUINT, FALSE,
			       next_invalid_group_id_list);
    }else{
      ags_audio_reset_group_id(audio,
			       current->first_recycling, current->last_recycling,
			       NULL, NULL, NULL,
			       NULL,
			       NULL, G_MAXUINT, FALSE,
			       invalid_group_id_list);
    }
    
    /* go down */
    ags_channel_recursive_unset_group_id_down_input(current,
						    next_invalid_group_id_list, next_invalid_child_group_id_list);

    /* free allocated lists */
    if(new_level){
      g_list_free(next_invalid_child_group_id_list);
    }

  }
  void ags_channel_tillrecycling_unset_group_id_up(AgsChannel *channel,
						   GList *invalid_group_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL)
      return;

    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      audio = AGS_AUDIO(current->audio);

      goto ags_channel_tillrecycling_reset_group_id_upOUTPUT;
    }
    
    while(current != NULL){
      audio = AGS_AUDIO(current->audio);

      /* input */
      ags_channel_reset_group_id(current,
				 NULL, NULL, NULL,
				 NULL,
				 NULL, G_MAXUINT,
				 invalid_group_id_list);


      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }      

      /* audio */
      ags_audio_reset_group_id(audio,
			       first_recycling, last_recycling,
			       NULL, NULL, NULL,
			       NULL,
			       NULL, G_MAXUINT, FALSE,
			       invalid_group_id_list);

      /* output */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output,
				  current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output,
				  current->line);
      }

    ags_channel_tillrecycling_reset_group_id_upOUTPUT:

      ags_audio_reset_group_id(audio,
			       first_recycling, last_recycling,
			       NULL, NULL, NULL,
			       NULL,
			       NULL, G_MAXUINT, TRUE,
			       invalid_group_id_list);


      ags_channel_reset_group_id(current,
				 NULL, NULL, NULL,
				 NULL,
				 NULL, G_MAXUINT,
				 invalid_group_id_list);

      current = current->link;
    }
  }
  void ags_channel_recursive_resolve_recall_down_input(AgsChannel *output,
						       GList *group_id, GList *child_group_id)
  {
    AgsAudio *audio;
    AgsChannel *current;
    GList *next_group_id, *next_child_group_id;
    gboolean new_level;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input,
				output->audio_channel);

      while(current != NULL){
	/* resolve input */
	ags_channel_recursive_reset_channel_resolve_recall(current,
							   group_id);

	if(current->link != NULL){
	  /* retrieve next child_group_id */
	  if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	    new_level = TRUE;

	    next_group_id = child_group_id;
	    next_child_group_id = ags_channel_group_id_to_child_group_id(current->link,
									 child_group_id);
	  }else{
	    new_level = FALSE;

	    next_group_id = group_id;
	    next_child_group_id = child_group_id;
	  }

	  /* go down */
	  ags_channel_recursive_resolve_recall_down(current->link,
						    next_group_id, next_child_group_id);

	  if(new_level){
	    g_list_free(next_group_id);
	  }
	}

	current = current->next_pad;
      }
    }else{
      current = ags_channel_nth(audio->input,
				output->line);

      /* resolve input */
      ags_channel_recursive_reset_channel_resolve_recall(current,
							 group_id);

      /* retrieve next child_group_id */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	new_level = TRUE;

	next_group_id = child_group_id;
	next_child_group_id = ags_channel_group_id_to_child_group_id(current->link,
								     child_group_id);
      }else{
	new_level = FALSE;

	next_group_id = group_id;
	next_child_group_id = child_group_id;
      }

      /* go down */
      ags_channel_recursive_resolve_recall_down(current->link,
						next_group_id, next_child_group_id);

      if(new_level){
	g_list_free(next_group_id);
      }
    }
  }
  void ags_channel_recursive_resolve_recall_down(AgsChannel *current,
						 GList *group_id, GList *child_group_id)
  {
    AgsAudio *audio;
    GList *next_group_id, *next_child_group_id;
    gboolean new_level;

    if(current == NULL)
      return;

    audio = AGS_AUDIO(current->audio);

    /* resolve output */
    ags_channel_recursive_reset_channel_resolve_recall(current,
						       group_id);
    
    /* retrieve next child_group_id */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      new_level = TRUE;

      next_group_id = child_group_id;
      next_child_group_id = ags_channel_group_id_to_child_group_id(current->link,
								   child_group_id);
    }else{
      new_level = FALSE;

      next_group_id = group_id;
      next_child_group_id = child_group_id;
    }
        
    /* resolve audio */
    if(new_level){
      ags_channel_recursive_reset_audio_resolve_recall(audio,
						       ags_output_find_first_input_recycling(AGS_OUTPUT(current)), ags_output_find_last_input_recycling(AGS_OUTPUT(current)),
						       group_id);
    }

    ags_channel_recursive_reset_audio_resolve_recall(audio,
						     ags_output_find_first_input_recycling(AGS_OUTPUT(current)), ags_output_find_last_input_recycling(AGS_OUTPUT(current)),
						     next_group_id);
    
    /* go down */
    ags_channel_recursive_resolve_recall_down_input(current,
						    next_group_id, next_child_group_id);

    if(new_level){
      g_list_free(next_group_id);
    }
  }
  void ags_channel_tillrecycling_resolve_recall_up(AgsChannel *channel,
						   GList *group_id)
  {
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL)
      return;

    audio = AGS_AUDIO(channel->audio);
    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_tillrecycling_resolve_recall_upOUTPUT;
    }

    while(current != NULL){
      audio = AGS_AUDIO(current->audio);

      /* resolve input */
      ags_channel_recursive_reset_channel_resolve_recall(current,
							 group_id);
      

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      /* resolve audio */
      ags_channel_recursive_reset_audio_resolve_recall(audio,
						       first_recycling, last_recycling,
						       group_id);


      /* resolve output */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output,
				  current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output,
				  current->line);
      }

    ags_channel_tillrecycling_resolve_recall_upOUTPUT:

      ags_channel_recursive_reset_channel_resolve_recall(current,
							 group_id);


      current = current->link;
    }
  }
  void ags_channel_recursive_init_recall_down_input(AgsChannel *output,
						    GList *group_id, GList *child_group_id)
  {
    AgsAudio *audio;
    AgsChannel *current;
    GList *next_group_id, *next_child_group_id;
    gboolean new_level;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
      new_level = TRUE;
    }else{
      new_level = FALSE;
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input, output->audio_channel);

      while(current != NULL){
	/* retrieve next AgsGroupIds */
	if(new_level){
	  next_group_id = child_group_id;
	  next_child_group_id = ags_channel_find_child_group_id_from_child(current,
									   child_group_id);
	}else{
	  next_group_id = group_id;
	  next_child_group_id = child_group_id;
	}

	/* go down */
	ags_channel_recursive_init_recall_down(current->link,
					       next_group_id, next_child_group_id);

	/* init AgsInput */
	ags_channel_recursive_reset_channel_init_recall(current,
							group_id);

	/* free allocated lists */
	if(new_level){
	  g_list_free(next_child_group_id);
	}

	current = current->next_pad;
      }
    }else{
      current = ags_channel_nth(audio->input, output->line);

      /* retrieve next AgsGroupIds */
      if(new_level){
	next_group_id = child_group_id;
	next_child_group_id = ags_channel_find_child_group_id_from_child(current,
									 child_group_id);
      }else{
	next_group_id = group_id;
	next_child_group_id = child_group_id;
      }
      
      /* go down */
      ags_channel_recursive_init_recall_down(current->link,
					     next_group_id, next_child_group_id);

      /* init AgsInput */
      ags_channel_recursive_reset_channel_init_recall(current,
						      group_id);

      /* free allocated lists */
      if(new_level){
	g_list_free(next_child_group_id);
      }
    }
  }
  void ags_channel_recursive_init_recall_down(AgsChannel *current,
					      GList *group_id, GList *child_group_id)
  {
    AgsAudio *audio;
    GList *next_group_id, *next_child_group_id;
    gboolean new_level;

    if(current == NULL)
      return;

    audio = AGS_AUDIO(current->audio);

    /* retrieve next child_group_id */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      new_level = TRUE;

      next_group_id = child_group_id;
      next_child_group_id = ags_channel_group_id_to_child_group_id(current->link,
								   child_group_id);
    }else{
      new_level = FALSE;

      next_group_id = group_id;
      next_child_group_id = child_group_id;
    }
    
    /* go down */
    ags_channel_recursive_resolve_recall_down_input(current,
						    next_group_id, next_child_group_id);

    /* init audio */
    if(new_level){
      ags_channel_recursive_reset_audio_init_recall(audio,
						    ags_output_find_first_input_recycling(AGS_OUTPUT(current)), ags_output_find_last_input_recycling(AGS_OUTPUT(current)),
						    group_id);
    }

    ags_channel_recursive_reset_audio_init_recall(audio,
						  ags_output_find_first_input_recycling(AGS_OUTPUT(current)), ags_output_find_last_input_recycling(AGS_OUTPUT(current)),
						  next_group_id);

    /* init output */
    ags_channel_recursive_reset_channel_init_recall(current,
						    group_id);

    if(new_level){
      g_list_free(next_group_id);
    }
  }
  void ags_channel_tillrecycling_init_recall_up(AgsChannel *channel,
						GList *group_id)
  {
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL)
      return;

    audio = AGS_AUDIO(channel->audio);
    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_tillrecycling_init_recall_upOUTPUT;
    }

    while(current != NULL){
      audio = AGS_AUDIO(current->audio);

      /* init input */
      ags_channel_recursive_reset_channel_init_recall(current,
						      group_id);
      

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      /* init audio */
      ags_channel_recursive_reset_audio_init_recall(audio,
						    first_recycling, last_recycling,
						    group_id);


      /* init output */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output,
				  current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output,
				  current->line);
      }

    ags_channel_tillrecycling_init_recall_upOUTPUT:

      ags_channel_recursive_reset_channel_init_recall(current,
						      group_id);


      current = current->link;
    }

  }

  /* entry point */
  if(channel == NULL && link == NULL)
    return;

  if(AGS_IS_OUTPUT(link) || AGS_IS_INPUT(channel)){
    AgsChannel *tmp, *tmp_old;

    tmp = link;
    link = channel;
    channel = tmp;

    tmp_old = old_link_link;
    old_link_link = old_channel_link;
    old_channel_link = tmp_old;
  }

  if(channel != NULL){
    first_recycling = channel->first_recycling;
    last_recycling = channel->last_recycling;
  }

  /* collect AgsDevoutPlays as lists */
  /* go down */
  channel_lower_devout_play_list = NULL;
  channel_lower_devout_play_list = ags_channel_tillrecycling_collect_devout_play_down(channel,
										      channel_lower_devout_play_list, FALSE);

  /* go up */
  link_upper_devout_play_list = NULL;
  link_upper_devout_play_list = ags_channel_recursive_collect_devout_play_up(link,
									     FALSE,
									     TRUE, &audio_signal_level);

  /* retrieve group_id related lists */
  /* generate parent_group_id list*/
  channel_parent_group_id_list = ags_channel_generate_list_with_toplevel_parent_group_id(channel_lower_devout_play_list);
  link_parent_group_id_list = ags_channel_generate_list_with_toplevel_parent_group_id(link_upper_devout_play_list);

  /* retrieve lower */
  channel_group_id_list = NULL;
  channel_child_group_id_list = NULL;

  channel_group_id_list = ags_channel_devout_play_to_group_id(channel_lower_devout_play_list);

  channel_child_group_id_list = ags_channel_group_id_to_child_group_id(channel,
								       channel_group_id_list);

  /* retrieve upper */
  link_group_id_list = NULL;
  link_child_group_id_list = NULL;

  link_group_id_list = ags_channel_devout_play_to_group_id(link_upper_devout_play_list);

  link_child_group_id_list = ags_channel_group_id_to_child_group_id(link,
								    link_group_id_list);

  /* retrieve invalid lower */
  invalid_lower_group_id_list = NULL;
  invalid_lower_child_group_id_list = NULL;

  invalid_lower_group_id_list = ags_channel_tillrecycling_collect_devout_play_down(old_link_link,
										   invalid_lower_group_id_list, TRUE);
  invalid_lower_child_group_id_list = ags_channel_group_id_to_child_group_id(old_link_link,
									     invalid_lower_group_id_list);

  /* retrieve invalid upper */
  invalid_upper_group_id_list = NULL;
  invalid_upper_child_group_id_list = NULL;

  link_upper_devout_play_list = ags_channel_recursive_collect_devout_play_up(old_channel_link,
									     TRUE,
									     FALSE, NULL);
  invalid_upper_child_group_id_list = ags_channel_group_id_to_child_group_id(old_channel_link,
									     link_upper_devout_play_list);

  /* reset group ids */
  /* go down */
  ags_channel_recursive_reset_group_id_down(channel,
					    link_parent_group_id_list, link_group_id_list, link_child_group_id_list,
					    link_upper_devout_play_list, audio_signal_level,
					    invalid_lower_group_id_list, invalid_lower_child_group_id_list);

  /* go up */
  ags_channel_tillrecycling_reset_group_id_up(link,
					      channel_parent_group_id_list, channel_group_id_list, channel_child_group_id_list,
					      channel_lower_devout_play_list,
					      invalid_upper_group_id_list);

  /* unset group ids */  
  /* go down */
  ags_channel_recursive_unset_group_id_down(old_link_link,
					    invalid_lower_group_id_list, invalid_lower_child_group_id_list);

  /* go up */
  ags_channel_tillrecycling_unset_group_id_up(old_channel_link,
					      invalid_upper_group_id_list);

  /* resolve recalls */
  /* go down */
  ags_channel_recursive_resolve_recall_down(channel,
					    link_group_id_list, link_child_group_id_list);

  /* go up */
  ags_channel_tillrecycling_resolve_recall_up(link,
					      channel_group_id_list);

  /* init recalls */
  for(stage = 0; stage < 3; stage++){
    /* go down */
    ags_channel_recursive_init_recall_down(channel,
					   link_group_id_list, link_child_group_id_list);

    /* go up */
    ags_channel_tillrecycling_init_recall_up(link,
					     channel_group_id_list);

  }

  /* free the lists */
  g_list_free(channel_lower_devout_play_list);
  g_list_free(link_upper_devout_play_list);

  g_list_free(channel_parent_group_id_list);
  g_list_free(channel_group_id_list);
  g_list_free(channel_child_group_id_list);

  g_list_free(link_parent_group_id_list);
  g_list_free(link_group_id_list);
  g_list_free(link_child_group_id_list);

  g_list_free(audio_signal_level);

  g_list_free(invalid_upper_group_id_list);
  g_list_free(invalid_upper_child_group_id_list);

  g_list_free(invalid_lower_group_id_list);
  g_list_free(invalid_lower_child_group_id_list);
}

AgsChannel*
ags_channel_new(GObject *audio)
{
  AgsChannel *channel;

  channel = (AgsChannel *) g_object_new(AGS_TYPE_CHANNEL,
					"audio\0", audio,
					NULL);

  return(channel);
}
