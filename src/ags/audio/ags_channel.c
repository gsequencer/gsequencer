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

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
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
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(channel->audio == audio)
	return;

      if(channel->audio != NULL){
	g_object_unref(G_OBJECT(channel->audio));
      }

      if(audio != NULL){
	g_object_ref(G_OBJECT(audio));
      }

      channel->audio = (GObject *) audio;
    }
    break;
  case PROP_DEVOUT:
    {
      AgsDevout *devout;

      devout = (AgsDevout *) g_value_get_object(value);

      ags_channel_set_devout(channel, (GObject *) devout);
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

#ifdef AGS_DEBUG
  g_message("connecting channel\0");
#endif

  //  ags_connectable_add_to_registry(connectable);

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

  if(recycling != NULL){
    while(recycling != channel->last_recycling->next){
      ags_connectable_connect(AGS_CONNECTABLE(recycling));
      
      recycling = recycling->next;
    }
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

  //FIXME:JK: wrong ref count
  if(channel->audio != NULL)
    //    g_object_unref(channel->audio);

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
  g_object_ref(recall);

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

  if(channel->link == link){
    return;
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
  AgsRecyclingContainer *recycling_container;
  GList *recall_id;
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
    //TODO:JK: check for compliance
    //    ags_recall_id_reset_recycling(output->recall_id,
    //				  output->first_recycling,
    //				  replace_with_first_recycling, replace_with_last_recycling);

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

  /* set recycling - update AgsChannel */
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
    nth_recycling = old_first_recycling;

    while(nth_recycling != old_last_recycling->next){
      next_recycling = nth_recycling->next;
      
      g_object_unref(G_OBJECT(nth_recycling));
      
      nth_recycling = next_recycling;
    }
  }
  
  /* set parent */
  if(first_recycling != NULL){
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

  /* update recycling container */
  if(old_last_recycling != NULL){
    recall_id = audio->recall_id;
    
    while(recall_id != NULL){
      recycling_container = (AgsRecyclingContainer *) AGS_RECALL_ID(recall_id->data)->recycling_container;
      
      nth_recycling = old_first_recycling;

      while(nth_recycling != old_last_recycling->next){
	ags_recycling_container_remove(recycling_container,
				       nth_recycling);
	
	nth_recycling = nth_recycling->next;
      } 
    
      recall_id = recall_id->next;
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
  if(recall_id->recycling_container->parent == NULL){
    list_recall = channel->play;
  }else{
    list_recall = channel->recall;
  }
  
  while((list_recall = ags_recall_find_recycling_container(list_recall, G_OBJECT(recall_id->recycling_container))) != NULL){
    recall = AGS_RECALL(list_recall->data);

    ags_recall_resolve_dependencies(recall);

    list_recall = list_recall->next;
  }
}

void
ags_channel_play(AgsChannel *channel,
		 AgsRecallID *recall_id,
		 gint stage)
{
  AgsRecall *recall;
  GList *list, *list_next;
  
  if(recall_id->recycling_container->parent != NULL)
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
							    AgsRecallID *default_recall_id);
  auto void ags_channel_recursive_play_output_threaded(AgsChannel *output, AgsRecallID *output_recall_id);
  auto void ags_channel_recursive_play_up_threaded(AgsChannel *channel, AgsRecallID *recall_id);

  void ags_channel_recursive_play_input_sync_threaded(AgsChannel *input, AgsRecallID *input_recall_id){
    AgsAudio *audio;
    AgsRecallID *child_recall_id;

    audio = AGS_AUDIO(input->audio);

    /* check if we go down */
    if(input->link != NULL){
      gint child_position;

      /* check if there is a new recycling */
      child_position = ags_recycling_container_find_child(input_recall_id->recycling_container,
							  input->first_recycling);
      
      if(child_position == -1){
	child_recall_id = ags_recall_id_find_recycling_container(input->link->recall_id,
								 input_recall_id->recycling_container);
      }else{
	GList *list;

	list = g_list_nth(input_recall_id->recycling_container->children,
			  child_position);

	if(list != NULL){
	  child_recall_id = ags_recall_id_find_recycling_container(AGS_AUDIO(input->link)->recall_id,
								   AGS_RECYCLING_CONTAINER(list->data));
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

    ags_iterator_thread_children_ready(AGS_ITERATOR_THREAD(AGS_RECYCLING_THREAD(input->recycling_thread)->iterator_thread),
				       AGS_THREAD(input->recycling_thread));
  }
  void ags_channel_recursive_play_input_async_threaded(AgsChannel *input,
						       AgsRecallID *default_recall_id){
    AgsAudio *audio;
    AgsRecallID *input_recall_id, *child_recall_id;

    /* iterate AgsInputs */
    while(input != NULL){
      gint child_position;

      /* input_recall_id - check if there is a new recycling */
      child_position = ags_recycling_container_find_child(default_recall_id->recycling_container,
							  input->first_recycling);
      
      if(child_position == -1){
	input_recall_id = ags_recall_id_find_recycling_container(input->recall_id,
								 default_recall_id->recycling_container);
      }else{
	GList *list;

	list = g_list_nth(default_recall_id->recycling_container->children,
			  child_position);

	if(list != NULL){
	  input_recall_id = ags_recall_id_find_recycling_container(input->recall_id,
								   AGS_RECYCLING_CONTAINER(list->data));
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
	  child_recall_id = ags_recall_id_find_recycling_container(input->link->recall_id,
								   input_recall_id->recycling_container);
	}else{
	  GList *list;

	  list = g_list_nth(default_recall_id->recycling_container->children,
			    child_position);

	  if(list != NULL){
	    child_recall_id = ags_recall_id_find_recycling_container(input->link->recall_id,
								     AGS_RECYCLING_CONTAINER(list->data));
	  }else{
	    child_recall_id = NULL;
	  }
	}

	/* follow the links */
	ags_channel_recursive_play_output_threaded(input->link,
						   child_recall_id);
      }
      
      /* call input */
      ags_recycling_thread_play_channel(AGS_RECYCLING_THREAD(AGS_CHANNEL(input)->recycling_thread),
					G_OBJECT(input),
					input_recall_id,
					stage);
      
      /* iterate */
      input = input->next_pad;
    }

    ags_iterator_thread_children_ready(AGS_ITERATOR_THREAD(AGS_RECYCLING_THREAD(input->recycling_thread)->iterator_thread),
				       AGS_THREAD(input->recycling_thread));
  }
  void ags_channel_recursive_play_output_threaded(AgsChannel *output, AgsRecallID *output_recall_id){
    AgsAudio *audio;
    AgsChannel *input;
    AgsRecallID *default_recall_id;
    gint child_position;
    
    audio = AGS_AUDIO(output->audio);
    
    /* call input */
    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      input = ags_channel_nth(audio->input,
			      output->audio_channel);

      //      if(ags_recall_id_get_run_stage(output_recall_id, stage)){
	/* block sync|async for this run */
      //	ags_channel_recall_id_set(output, output_recall_id, TRUE,
      //			  AGS_CHANNEL_RECALL_ID_RUN_STAGE,
      //			  "stage\0", stage,
      //			  NULL);


	/* input_recall_id - check if there is a new recycling */
	child_position = ags_recycling_container_find_child(output_recall_id->recycling_container,
							    input->first_recycling);
      
	if(child_position == -1){
	  default_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
								     output_recall_id->recycling_container);
	}else{
	  GList *list;

	  list = g_list_nth(output_recall_id->recycling_container->children,
			    child_position);

	  if(list != NULL){
	    default_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
								       AGS_RECYCLING_CONTAINER(list->data));
	  }else{
	    default_recall_id = NULL;
	  }
	}

	/* run in AGS_AUDIO_ASYNC mode */
	ags_channel_recursive_play_input_async_threaded(input,
							default_recall_id);
	//      }else{
	/* unblock sync|async for next run */
	//	if((AGS_CHANNEL_RUNNING & (output->flags)) != 0){
	//	  ags_recall_id_unset_run_stage(output_recall_id, stage);
	//	}
	//      }

      /* call audio */
      if(default_recall_id->recycling_container != output_recall_id->recycling_container){
	ags_recycling_thread_play_audio(AGS_RECYCLING_THREAD(AGS_CHANNEL(output)->recycling_thread),
					G_OBJECT(output), G_OBJECT(audio),
					default_recall_id,
					stage);
      }

      ags_recycling_thread_play_audio(AGS_RECYCLING_THREAD(AGS_CHANNEL(output)->recycling_thread),
				      G_OBJECT(output), G_OBJECT(audio),
				      output_recall_id,
				      stage);
    }else{
      input = ags_channel_nth(audio->input,
			      output->line);
	
      /* input_recall_id - check if there is a new recycling */
      child_position = ags_recycling_container_find_child(output_recall_id->recycling_container,
							  input->first_recycling);
      
      if(child_position == -1){
	default_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
								   output_recall_id->recycling_container);
      }else{
	GList *list;

	list = g_list_nth(output_recall_id->recycling_container->children,
			  child_position);

	if(list != NULL){
	  default_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
								     AGS_RECYCLING_CONTAINER(list->data));
	}else{
	  default_recall_id = NULL;
	}
      }

      /* traverse the tree */
      ags_channel_recursive_play_input_sync_threaded(input,
						     default_recall_id);

      /* call audio */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	ags_recycling_thread_play_audio(AGS_RECYCLING_THREAD(output->recycling_thread),
					G_OBJECT(output), G_OBJECT(audio),
					default_recall_id,
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
      recall_id = ags_recall_id_find_recycling_container(channel->recall_id,
							 recall_id->recycling_container);

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

      recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							 recall_id->recycling_container);

      /* get output */
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0)
	channel = ags_channel_nth(audio->output, channel->audio_channel);
      else
	channel = ags_channel_nth(audio->output, channel->line);

      ags_recycling_thread_play_audio(AGS_RECYCLING_THREAD(channel->recycling_thread),
				      G_OBJECT(channel), G_OBJECT(audio),
				      recall_id,
				      stage);

      /* play output */
      recall_id = ags_recall_id_find_recycling_container(channel->recall_id,
							 recall_id->recycling_container);

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
  auto void ags_channel_recursive_play_input_sync(AgsChannel *input, AgsRecallID *input_recall_id);
  auto void ags_channel_recursive_play_input_async(AgsChannel *input,
						   AgsRecallID *default_recall_id);
  auto void ags_channel_recursive_play_output(AgsChannel *output, AgsRecallID *output_recall_id);
  auto void ags_channel_recursive_play_up(AgsChannel *channel, AgsRecallID *recall_id);

  void ags_channel_recursive_play_input_sync(AgsChannel *input, AgsRecallID *input_recall_id){
    AgsAudio *audio;
    AgsRecallID *child_recall_id;

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
	child_recall_id = ags_recall_id_find_recycling_container(input->link->recall_id,
								 input_recall_id->recycling_container);
      }else{
	GList *list;

	list = g_list_nth(input_recall_id->recycling_container->children,
			  child_position);

	if(list != NULL){
	  child_recall_id = ags_recall_id_find_recycling_container(input->link->recall_id,
								   AGS_RECYCLING_CONTAINER(list->data));
	}else{
	  child_recall_id = NULL;
	}
      }
      
      /* go down */
      ags_channel_recursive_play_output(input->link,
					child_recall_id);
    }
      
    /* call input */
    ags_channel_play(input,
		     input_recall_id,
		     stage);
  }
  void ags_channel_recursive_play_input_async(AgsChannel *input,
					      AgsRecallID *default_recall_id){
    AgsRecallID *input_recall_id;

    if(input == NULL){
      return;
    }

    /* iterate AgsInputs */
    while(input != NULL){
      gint child_position;

      /* input_recall_id - check if there is a new recycling */
      child_position = ags_recycling_container_find_child(default_recall_id->recycling_container,
							  input->first_recycling);
      
      if(child_position == -1){
	input_recall_id = ags_recall_id_find_recycling_container(input->recall_id,
								 default_recall_id->recycling_container);
      }else{
	GList *list;

	list = g_list_nth(default_recall_id->recycling_container->children,
			  child_position);

	if(list != NULL){
	  input_recall_id = ags_recall_id_find_recycling_container(input->recall_id,
								   AGS_RECYCLING_CONTAINER(list->data));
	}else{
	  input_recall_id = NULL;
	}
      }

      /* from async to sync */
      ags_channel_recursive_play_input_sync(input,
					    input_recall_id);

      /* iterate */
      input = input->next_pad;
    }
  }
  void ags_channel_recursive_play_output(AgsChannel *output, AgsRecallID *output_recall_id){
    AgsAudio *audio;
    AgsChannel *input;
    AgsRecallID *audio_recall_id, *default_recall_id;
    gint child_position;
    
    if(output == NULL){
      return;
    }

    audio = AGS_AUDIO(output->audio);
    
    /* input_recall_id - check if there is a new recycling */
    child_position = ags_recycling_container_find_child(output_recall_id->recycling_container,
							audio->input->first_recycling);
      
    if(child_position == -1){
      default_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
								 output_recall_id->recycling_container);
    }else{
      GList *list;

      list = g_list_nth(output_recall_id->recycling_container->children,
			child_position);

      if(list != NULL){
	default_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
								   AGS_RECYCLING_CONTAINER(list->data));
      }else{
	default_recall_id = NULL;
      }
    }
    
    /* call input */
    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      //      if(ags_recall_id_get_run_stage(output_recall_id, stage)){
      input = ags_channel_nth(audio->input,
			      output->audio_channel);
      
      //TODO:JK: optimize tree see deprecated AgsRunOrder
      /* block sync|async for this run */
	//	ags_channel_recall_id_set(output, output_recall_id, TRUE,
	//			  AGS_CHANNEL_RECALL_ID_RUN_STAGE,
	//			  "stage\0", stage,
	//			  NULL);
      
	/* traverse the tree - run in AGS_AUDIO_ASYNC mode */
      ags_channel_recursive_play_input_async(input,
					     default_recall_id);
      //      }else{
      /* unblock sync|async for next run */
      //	if((AGS_CHANNEL_RUNNING & (output->flags)) != 0){
      //TODO:JK: optimize tree see deprecated AgsRunOrder
      //	  ags_recall_id_unset_run_stage(output_recall_id, stage);
      //	}
    
      /* call audio */
      if(default_recall_id->recycling_container != output_recall_id->recycling_container){
	ags_audio_play(audio,
		       default_recall_id,
		       stage);
      }
    
      ags_audio_play(audio,
		     output_recall_id,
		     stage);
    }else{
      input = ags_channel_nth(audio->input,
			    output->line);
		  
      ags_channel_recursive_play_input_sync(input,
					    default_recall_id);

      /* call audio */
      if(default_recall_id->recycling_container != output_recall_id->recycling_container){
	ags_audio_play(audio,
		       default_recall_id,
		       stage);
      }

      ags_audio_play(audio,
		     output_recall_id,
		     stage);
    }
    
    /* call output */
    ags_channel_play(output,
		     output_recall_id,
		     stage);
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
      recall_id = ags_recall_id_find_recycling_container(channel->recall_id,
							 recall_id->recycling_container);

    ags_channel_recursive_play_up0:
      ags_channel_play(channel,
		       recall_id,
		       stage);

      /* play audio */
      ags_audio_play(audio,
		     recall_id,
		     stage);

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;

      recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							 recall_id->recycling_container);

      /* get output */
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0)
	channel = ags_channel_nth(audio->output, channel->audio_channel);
      else
	channel = ags_channel_nth(audio->output, channel->line);

      /* play output */
      recall_id = ags_recall_id_find_recycling_container(channel->recall_id,
							 recall_id->recycling_container);

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
 * @recall_id the #AgsRecallID the newly allocated #AgsRecall objects belongs to
 *
 * Duplicate #AgsRecall templates for use with ags_channel_recursive_play(),
 * but ags_channel_recursive_play_init() may call this function for you.
 */
void
ags_channel_duplicate_recall(AgsChannel *channel,
			     AgsRecallID *recall_id)
{
  AgsAudio *audio;
  AgsRecall *recall, *copy;
  GList *list_recall;
  gboolean playback, sequencer, notation;

  audio = AGS_AUDIO(channel->audio);

  if((AGS_RECALL_ID_PLAYBACK & (recall_id->flags)) != 0){
    playback = TRUE;
  }

  if((AGS_RECALL_ID_SEQUENCER & (recall_id->flags)) != 0){
    sequencer = TRUE;
  }

  if((AGS_RECALL_ID_NOTATION & (recall_id->flags)) != 0){
    notation = TRUE;
  }
  
  /* get the appropriate lists */
  if(recall_id->recycling_container->parent == NULL){
    list_recall = channel->play;
  }else{
    list_recall = channel->recall;
  }

  /*  */
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

#ifdef AGS_DEBUG
    g_message("recall duplicated: %s\n\0", G_OBJECT_TYPE_NAME(copy));
#endif
    
    /* set appropriate flag */
    if(playback){
      ags_recall_set_flags(copy, AGS_RECALL_PLAYBACK);
    }else if(sequencer){
      ags_recall_set_flags(copy, AGS_RECALL_SEQUENCER);
    }else if(notation){
      ags_recall_set_flags(copy, AGS_RECALL_NOTATION);
    }

    /* append to AgsAudio */
    if(recall_id->recycling_container->parent == NULL)
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
 * @recall_id the #AgsRecallId this recall belongs to
 *
 * Prepare #AgsRecall objects to become runnning, ags_channel_recursive_play_init()
 * may call this function for you.
 */
void
ags_channel_init_recall(AgsChannel *channel, gint stage,
			AgsRecallID *recall_id)
{
  AgsRecall *recall;
  GList *list_recall;
  
#ifdef AGS_DEBUG
  g_message("ags_channel_init_recall@%d - audio::IN[%u]; channel: %llu %llu\n\0",
	    stage,
	    AGS_AUDIO(channel->audio)->input_lines,
	    (long long unsigned int) channel->audio_channel,
	    (long long unsigned int) channel->pad);  
#endif

  if(recall_id->recycling_container->parent == NULL)
    list_recall = channel->play;
  else
    list_recall = channel->recall;

  while(list_recall != NULL){
    recall = AGS_RECALL(list_recall->data);
    
    if((AGS_RECALL_RUN_INITIALIZED & (recall->flags)) != 0 ||
       recall->recall_id == NULL ||
       recall->recall_id->recycling_container != recall_id->recycling_container ||
       AGS_IS_RECALL_CHANNEL(recall)){
      list_recall = list_recall->next;
      continue;
    }
    
    if((AGS_RECALL_TEMPLATE & (recall->flags)) == 0){
#ifdef AGS_DEBUG
      g_message("recall run init: %s\n\0", G_OBJECT_TYPE_NAME(recall));
#endif

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
 * @arrange_recall_id %TRUE if new #AgsRecallID objects should be created
 * @duplicate_templates %TRUE if the #AgsRecall templates should be duplicated
 * @playback %TRUE if the purpose is a simple playback of the tree, this option is used to omit the
 * duplication of #AgsRecall templates which haven't set @AGS_RECALL_PLAYBACK flag
 * @sequencer %TRUE if the purpose is playing the tree for a sequencer, this option is used to omit the
 * duplication of #AgsRecall templates which haven't set the @AGS_RECALL_SEQUENCER flag
 * @notation %TRUE if the purpose is playing the tree for a notation, this option is used to omit the
 * duplication of #AgsRecall templates which haven't set the @AGS_RECALL_NOTATION flag
 * @resolve_dependencies %TRUE if the
 * @recall_id the initial recall id or %NULL
 *
 * Make the tree ready for a new #AgsDevoutPlay.
 */
AgsRecallID*
ags_channel_recursive_play_init(AgsChannel *channel, gint stage,
				gboolean arrange_recall_id,
				gboolean duplicate_templates, gboolean playback, gboolean sequencer, gboolean notation,
				gboolean resolve_dependencies,
				AgsRecallID *recall_id)
{
  AgsAudio *audio;
  AgsRecallID *audio_recall_id, *default_recall_id;
  gboolean stage_stop;

  auto void ags_channel_recursive_play_init_arrange_recall_id_up(AgsChannel *channel,
								 AgsRecallID *recall_id,
								 gboolean initially_unowned);
  auto void ags_channel_recursive_play_init_arrange_recall_id_down_input(AgsChannel *output,
									 AgsRecallID *recall_id,
									 gboolean initially_unowned);
  auto void ags_channel_recursive_play_init_arrange_recall_id_down(AgsChannel *output,
								   AgsRecallID *recall_id,
								   gboolean initially_unowned);

  auto void ags_channel_recursive_play_init_duplicate_up(AgsChannel *channel,
							 AgsRecallID *recall_id);
  auto void ags_channel_recursive_play_init_duplicate_down_input(AgsChannel *output,
								 AgsRecallID *recall_id);
  auto void ags_channel_recursive_play_init_duplicate_down(AgsChannel *output,
							   AgsRecallID *recall_id);

  auto void ags_channel_recursive_play_init_resolve_up(AgsChannel *channel,
						       AgsRecallID *recall_id);
  auto void ags_channel_recursive_play_init_resolve_down_input(AgsChannel *output,
							       AgsRecallID *recall_id);
  auto void ags_channel_recursive_play_init_resolve_down(AgsChannel *output,
							 AgsRecallID *recall_id);
  
  auto void ags_channel_recursive_play_init_up(AgsChannel *channel,
					       AgsRecallID *recall_id);
  auto void ags_channel_recursive_play_init_down_input(AgsChannel *output,
						       AgsRecallID *recall_id);
  auto void ags_channel_recursive_play_init_down(AgsChannel *output,
						 AgsRecallID *recall_id);
  
  /*
   * arrangeing recall ids is done from the axis to the root and then from the axis to the leafs
   */
  void ags_channel_recursive_play_init_arrange_recall_id_up(AgsChannel *channel,
							    AgsRecallID *recall_id,
							    gboolean initially_unowned)
  {
    AgsAudio *audio;
    AgsChannel *current;
    
    audio = AGS_AUDIO(channel->audio);
    
    current = channel;
    
    if(AGS_IS_OUTPUT(channel)){
      if(!initially_unowned){
	goto ags_channel_recursive_play_init_up_OUTPUT;
      }else{
	current = current->link;

	if(current == NULL){
	  return;
	}

	audio = AGS_AUDIO(current->audio);
      }
    }else{
      if(initially_unowned){
	if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	  current = ags_channel_nth(audio->output,
				    current->audio_channel);
	}else{
	  current = ags_channel_nth(audio->output,
				    current->line);
	}

	goto ags_channel_recursive_play_init_up_OUTPUT;
      }
    }

    /* goto toplevel AgsChannel */
    while(current != NULL){
      AgsRecallID *current_recall_id;

      /* AgsInput */
      current_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling\0", current->first_recycling,
				       "recycling_container\0", recall_id->recycling_container,
				       NULL);
      current->recall_id = ags_recall_id_add(current->recall_id,
					     current_recall_id);

      if(playback){
	current_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(sequencer){
	current_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(notation){
	current_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }

      /* AgsAudio */
      current_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling\0", current->first_recycling,
				       "recycling_container\0", recall_id->recycling_container,
				       NULL);
      audio->recall_id = ags_recall_id_add(audio->recall_id,
					   current_recall_id);

      if(playback){
	current_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(sequencer){
	current_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(notation){
	current_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }

      /* AgsOutput */
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

    ags_channel_recursive_play_init_up_OUTPUT:
      current_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling\0", current->first_recycling,
				       "recycling_container\0", recall_id->recycling_container,
				       NULL);
      current->recall_id = ags_recall_id_add(current->recall_id,
					     current_recall_id);

      if(playback){
	current_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(sequencer){
	current_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(notation){
	current_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }

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
  void ags_channel_recursive_play_init_arrange_recall_id_down_input(AgsChannel *output,
								    AgsRecallID *default_recall_id,
								    gboolean initially_unowned)
  {
    AgsAudio *audio;
    AgsChannel *input;
    AgsRecallID *input_recall_id;
    AgsRecyclingContainer *recycling_container;

    audio = AGS_AUDIO(output->audio);

    if(audio->input == NULL){
      return;
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      input = ags_channel_nth(audio->input, output->audio_channel);
    
      /* recall id */
      while(input != NULL){
	input_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling_container\0", default_recall_id->recycling_container,
				       "recycling\0", input->first_recycling,
				       NULL);
	input->recall_id = ags_recall_id_add(input->recall_id,
					     input_recall_id);

	if(playback){
	  input_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
	}

	if(sequencer){
	  input_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
	}

	if(notation){
	  input_recall_id->flags |= AGS_RECALL_ID_NOTATION;
	}

	/* follow the links */
	if(input->link != NULL){
	  ags_channel_recursive_play_init_arrange_recall_id_down(input->link,
								 input_recall_id,
								 FALSE);
	}

	/* iterate */
	input = input->next_pad;
      }
    }else{
      input = ags_channel_nth(audio->input, output->line);

      /* recall id */
      input_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling_container\0", default_recall_id->recycling_container,
				     "recycling\0", input->first_recycling,
				     NULL);
      input->recall_id = ags_recall_id_add(input->recall_id,
					   input_recall_id);

      if(playback){
	input_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(sequencer){
	input_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(notation){
	input_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }


      /* follow the links */
      if(input->link != NULL){
	ags_channel_recursive_play_init_arrange_recall_id_down(input->link,
							       input_recall_id,
							       FALSE);
      }
    }
  }
  void ags_channel_recursive_play_init_arrange_recall_id_down(AgsChannel *output,
							      AgsRecallID *recall_id,
							      gboolean initially_unowned)
  {
    AgsAudio *audio;
    AgsRecallID *output_recall_id, *audio_recall_id, *default_recall_id;

    if(output == NULL){
      return;
    }

    /* AgsOutput */
    /* recall id */
    if(!initially_unowned){
      output_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				      "recycling_container\0", recall_id->recycling_container,
				      "recycling\0", output->first_recycling,
				      NULL);
      output->recall_id = ags_recall_id_add(output->recall_id,
					    output_recall_id);
      if(playback){
	output_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(sequencer){
	output_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(notation){
	output_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }
    }else{
      output_recall_id = recall_id;
    }

    /* AgsAudio */
    audio = AGS_AUDIO(output->audio);
    
    /* recall id */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      AgsChannel *input, *last_input;
      AgsRecycling *recycling;
      AgsRecyclingContainer *recycling_container, *child_recycling_container;
      gint recycling_length;
      gint i;
    
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	input = ags_channel_nth(audio->input,
				output->audio_channel);
	last_input = ags_channel_pad_last(input);
      }else{
	input = ags_channel_nth(audio->input,
				output->line);
	last_input = input;
      }

      /* recall id */
      audio_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling_container\0", recall_id->recycling_container,
				     "recycling\0", output->first_recycling,
				    NULL);
      audio->recall_id = ags_recall_id_add(audio->recall_id,
					   audio_recall_id);

      if(playback){
	audio_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(sequencer){
	audio_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(notation){
	audio_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }

      /* recycling container */
      recycling_length = ags_recycling_position(input->first_recycling, last_input->last_recycling->next,
						last_input->last_recycling);
      recycling_length++;

      recycling_container = (AgsRecyclingContainer *) g_object_new(AGS_TYPE_RECYCLING_CONTAINER,
								   "parent\0", audio_recall_id->recycling_container,
								   "length\0", recycling_length,
								   NULL);
      
      default_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling_container\0", recycling_container,
				       "recycling\0", input->first_recycling,
				       NULL);
      audio->recall_id = ags_recall_id_add(audio->recall_id,
					   default_recall_id);

      if(playback){
	default_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(sequencer){
	default_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(notation){
	default_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }

      g_object_set(G_OBJECT(recycling_container),
		   "recall_id\0", default_recall_id,
		   NULL);

      recycling = input->first_recycling;

      for(i = 0; i < recycling_length; i++){
	recycling_container->recycling[i] = recycling;

	recycling = recycling->next;
      }
    }else{
      default_recall_id = audio_recall_id;
    }

    /* follow the links */
    ags_channel_recursive_play_init_arrange_recall_id_down_input(output,
								 default_recall_id,
								 FALSE);
  }

  /*
   * duplicateing template recalls is done from the root to the leafes
   *
   * externalized 
   * externalized 
   */

  void ags_channel_recursive_play_init_duplicate_up(AgsChannel *channel,
						    AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL){
      return;
    }
    
    audio = AGS_AUDIO(channel->audio);

    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_recursive_play_init_duplicate_up_OUTPUT;
    }

    /* goto toplevel AgsChannel */
    while(current != NULL){
      /* AgsInput */
      recall_id = ags_recall_id_find_recycling_container(current->recall_id,
							 recall_id->recycling_container);

      ags_channel_duplicate_recall(current,
				   recall_id);

      /* AgsAudio */
      recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							 recall_id->recycling_container);

      ags_audio_duplicate_recall(audio,
				 recall_id);

      /* AgsOutput */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output, current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output, current->line);
      }

      recall_id = ags_recall_id_find_recycling_container(current->recall_id,
							 recall_id->recycling_container);

    ags_channel_recursive_play_init_duplicate_up_OUTPUT:
      ags_channel_duplicate_recall(current,
				   recall_id);

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
							    AgsRecallID *default_recall_id)
  {
    AgsAudio *audio;
    AgsChannel *input, *input_start;
    AgsRecallID *input_recall_id;

    audio = AGS_AUDIO(output->audio);

    if(audio->input == NULL){
      return;
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){ /* async order of channels within audio */
      /* retrieve input */
      input_start =
	input = ags_channel_nth(audio->input,
				output->audio_channel);
          
      /* duplicate recalls on input */
      while(input != NULL){
	input_recall_id = ags_recall_id_find_recycling_container(input->recall_id,
								 default_recall_id->recycling_container);

	/* duplicate input */
	ags_channel_duplicate_recall(input,
				     input_recall_id);

	/* iterate */
	input = input->next_pad;
      }

      /* traverse the tree */
      input = input_start;

      while(input != NULL){
	if(input->link != NULL){
	  input_recall_id = ags_recall_id_find_recycling_container(input->recall_id,
								   default_recall_id->recycling_container);

	  /* follow the links */
	  ags_channel_recursive_play_init_duplicate_down(input->link,
							 input_recall_id);
	}

	/* iterate */
	input = input->next_pad;
      }

    }else{ /* sync order of channels within audio */
      /* retrieve input */
      input = ags_channel_nth(audio->input, output->line);

      input_recall_id = ags_recall_id_find_recycling_container(input->recall_id,
							       default_recall_id->recycling_container);
      
      /* duplicate recalls on input */
      ags_channel_duplicate_recall(input,
				   input_recall_id);

      /* follow the links */
      if(input->link != NULL){
	/* follow the links */
	ags_channel_recursive_play_init_duplicate_down(input->link,
						       input_recall_id);
      }
    }
  }
  void ags_channel_recursive_play_init_duplicate_down(AgsChannel *output,
						      AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsRecallID *audio_recall_id, *default_recall_id;

    if(output == NULL){
      return;
    }

    /* AgsAudio */
    audio = AGS_AUDIO(output->audio);

    /* duplicate output */
    ags_channel_duplicate_recall(output,
				 recall_id);

    /* retrieve next recall id */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      AgsChannel *input;
      AgsRecyclingContainer *recycling_container;
      GList *list;

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	input = ags_channel_nth(audio->input,
				output->audio_channel);
      }else{
	input = ags_channel_nth(audio->input,
				output->line);
      }

      audio_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							       recall_id->recycling_container);

      list = g_list_nth(audio_recall_id->recycling_container->children,
			ags_recycling_container_find_child(audio_recall_id->recycling_container,
							   input->first_recycling));
      default_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
								 AGS_RECYCLING_CONTAINER(list->data));
    }else{
      audio_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							       recall_id->recycling_container);

      default_recall_id = audio_recall_id;
    }

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      /* duplicate audio */
      ags_audio_duplicate_recall(audio,
				 audio_recall_id);

      ags_audio_duplicate_recall(audio,
				 default_recall_id);

      /* call function which duplicates input */
      ags_channel_recursive_play_init_duplicate_down_input(output,
							   default_recall_id);
    }else{
      /* duplicate audio */
      ags_audio_duplicate_recall(audio,
				 audio_recall_id);

      /* call function which duplicates input */
      ags_channel_recursive_play_init_duplicate_down_input(output,
							   default_recall_id);
    }
  }

  /*
   * resolving recall dependencies has to be done from the root to the leafs
   */
  void ags_channel_recursive_play_init_resolve_up(AgsChannel *channel,
						  AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsChannel *current;
    
    if(channel == NULL){
      return;
    }

    audio = AGS_AUDIO(channel->audio);

    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_recursive_play_init_resolve_up0;
    }

    while(current != NULL){
      /* resolve input */
      recall_id = ags_recall_id_find_recycling_container(current->recall_id,
							 recall_id->recycling_container);

      ags_channel_resolve_recall(current,
				 recall_id);
      
      /* resolve audio */
      recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							 recall_id->recycling_container);

      ags_audio_resolve_recall(audio,
			       recall_id);

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
      recall_id = ags_recall_id_find_recycling_container(channel->recall_id,
							 recall_id->recycling_container);

    ags_channel_recursive_play_init_resolve_up0:
      ags_channel_resolve_recall(current,
				 recall_id);


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
							  AgsRecallID *default_recall_id)
  {
    AgsAudio *audio;
    AgsChannel *input, *input_start;
    AgsRecallID *input_recall_id;

    audio = AGS_AUDIO(output->audio);

    if(audio->input == NULL){
      return;
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){ /* async order of channels within audio */
      /* retrieve input */
      input_start =
	input = ags_channel_nth(audio->input, output->audio_channel);
      
      /* resolve recall dependencies on input */
      while(input != NULL){
	input_recall_id = ags_recall_id_find_recycling_container(input->recall_id,
								 default_recall_id->recycling_container);

	/* resolve input */
	ags_channel_resolve_recall(input,
				   input_recall_id);

	/* iterate */
	input = input->next_pad;
      }

      /* traverse the tree */
      input = input_start;

      while(input != NULL){
	if(input->link != NULL){
	  input_recall_id = ags_recall_id_find_recycling_container(input->recall_id,
								   default_recall_id->recycling_container);

	  /* follow the links */
	  ags_channel_recursive_play_init_resolve_down(input->link,
						       input_recall_id);
	}

	/* iterate */
	input = input->next_pad;
      }
    }else{ /* sync order of channels within audio */
      /* retrieve input */
      input = ags_channel_nth(audio->input, output->line);

      input_recall_id = ags_recall_id_find_recycling_container(input->recall_id,
							       default_recall_id->recycling_container);

      /* resolve recall dependencies on input */
      ags_channel_resolve_recall(input,
				 input_recall_id);

      /* traverse the tree */
      if(input->link != NULL){

	/* follow the links */
	ags_channel_recursive_play_init_resolve_down(input->link,
						     input_recall_id);
      }
    }
  }
  void ags_channel_recursive_play_init_resolve_down(AgsChannel *output,
						    AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsChannel *input;
    AgsRecallID *audio_recall_id, *default_recall_id;

    if(output == NULL){
      return;
    }

    /* AgsAudio */
    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      input = ags_channel_nth(audio->input,
			      output->audio_channel);
    }else{
      input = ags_channel_nth(audio->input,
			      output->line);
    }

    /* retrieve next recall id */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      GList *list;

      audio_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							       recall_id->recycling_container);

      list = g_list_nth(audio_recall_id->recycling_container->children,
			ags_recycling_container_find_child(AGS_RECYCLING_CONTAINER(audio_recall_id->recycling_container),
							   input->first_recycling));
      default_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							       AGS_RECYCLING_CONTAINER(list->data));
    }else{
      audio_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							       recall_id->recycling_container);

      default_recall_id = audio_recall_id;
    }

    /* resolve dependencies on output */
    ags_channel_resolve_recall(output,
			       recall_id);
    
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      /* resolve dependencies on audio */
      ags_audio_resolve_recall(audio,
			       audio_recall_id);
      
      ags_audio_resolve_recall(audio,
			       default_recall_id);

      /* traverse the tree */
      ags_channel_recursive_play_init_resolve_down_input(output,
							 default_recall_id);
    }else{
      /* resolve dependencies on audio */
      ags_audio_resolve_recall(audio,
			       audio_recall_id);

      /* traverse the tree */
      ags_channel_recursive_play_init_resolve_down_input(output,
							 default_recall_id);
    }
  }

  /*
   * run-initializing recalls is done from the leafs to the root
   */
  void ags_channel_recursive_play_init_up(AgsChannel *channel,
					  AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL){
      return;
    }

    audio = AGS_AUDIO(channel->audio);

    current = channel;
    
    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_recursive_play_init_up0;
    }

    /* goto toplevel AgsChannel */
    while(current != NULL){
      //NOTE:JK: see documentation
      //      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      //	break;

      /* AgsInput */
      recall_id = ags_recall_id_find_recycling_container(current->recall_id,
							 recall_id->recycling_container);

      ags_channel_init_recall(current, stage,
			      recall_id);
      
      /* AgsAudio */
      recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							 recall_id->recycling_container);

      ags_audio_init_recall(audio, stage,
			    recall_id);

      /* AgsOutput */
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	current = ags_channel_nth(audio->output, current->audio_channel);
      }else{
	current = ags_channel_nth(audio->output, current->line);
      }

      recall_id = ags_recall_id_find_recycling_container(current->recall_id,
							 recall_id->recycling_container);

    ags_channel_recursive_play_init_up0:
      ags_channel_init_recall(current, stage,
			      recall_id);

      /* iterate */      
      if(current->link == NULL){
	break;
      }
    
      audio = AGS_AUDIO(current->link->audio);
            
      current = current->link;
    }
  }
  void ags_channel_recursive_play_init_down_input(AgsChannel *output,
						  AgsRecallID *default_recall_id)
  {
    AgsAudio *audio;
    AgsChannel *input;
    AgsRecallID *input_recall_id;
    
    audio = AGS_AUDIO(output->audio);

    if(audio->input == NULL){
      return;
    }

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      input = ags_channel_nth(audio->input,
			      output->audio_channel);
    
      while(input != NULL){
	input_recall_id = ags_recall_id_find_recycling_container(input->recall_id,
								 default_recall_id->recycling_container);

	/* follow the links */
	if(input->link != NULL){
	  ags_channel_recursive_play_init_down(input->link,
					       input_recall_id);
	}
	
	/* init recall */
	ags_channel_init_recall(input, stage,
				input_recall_id);

	/* iterate */
	input = input->next_pad;
      }
    }else{
      input = ags_channel_nth(audio->input,
			      output->line);

      input_recall_id = ags_recall_id_find_recycling_container(input->recall_id,
							       default_recall_id->recycling_container);

      /* follow the links */
      if(input->link != NULL){
	ags_channel_recursive_play_init_down(input->link,
					     input_recall_id);
      }

      /* init recall */
      ags_channel_init_recall(input, stage,
			      input_recall_id);
    }
  }
  void ags_channel_recursive_play_init_down(AgsChannel *output,
					    AgsRecallID *recall_id)
  {
    AgsAudio *audio;
    AgsRecallID *default_recall_id, *audio_recall_id;

    if(output == NULL){
      return;
    }

    /* AgsAudio */
    audio = AGS_AUDIO(output->audio);

    /* retrieve next recall id */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      AgsChannel *input;
      GList *list;

      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	input = ags_channel_nth(audio->input,
				output->audio_channel);
      }else{
	input = ags_channel_nth(audio->input,
				output->line);
      }

      audio_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							       recall_id->recycling_container);

      list = g_list_nth(recall_id->recycling_container->children,
			ags_recycling_container_find_child(AGS_RECYCLING_CONTAINER(audio_recall_id->recycling_container),
							   input->first_recycling));
      default_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							       AGS_RECYCLING_CONTAINER(list->data));
    }else{
      audio_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							       recall_id->recycling_container);

      default_recall_id = audio_recall_id;
    }


    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      ags_audio_init_recall(audio, stage,
			    audio_recall_id);

      ags_audio_init_recall(audio, stage,
			    default_recall_id);

      /* follow the links */
      ags_channel_recursive_play_init_down_input(output,
						 default_recall_id);
    }else{
      ags_audio_init_recall(audio, stage,
			    audio_recall_id);

      /* follow the links */
      ags_channel_recursive_play_init_down_input(output,
						 default_recall_id);
    }

    /* init recall*/
    ags_channel_init_recall(output, stage,
			    recall_id);
  }


  /*
   * entry point
   */
  if(channel == NULL){
    return;
  }

  audio = AGS_AUDIO(channel->audio);

  /* arrange recall id */
  if(arrange_recall_id){
    if(recall_id == NULL){
      AgsRecycling *recycling;
      AgsRecyclingContainer *recycling_container;
      gint recycling_length;
      gint i;

      /* recycling container */
      recycling_length = ags_recycling_position(channel->first_recycling, channel->last_recycling->next,
						channel->last_recycling);
      recycling_length++;

      recycling_container = (AgsRecyclingContainer *) g_object_new(AGS_TYPE_RECYCLING_CONTAINER,
								   "length\0", recycling_length,
								   NULL);
      
      /* recall id */
      recall_id = g_object_new(AGS_TYPE_RECALL_ID,
			       "recycling_container\0", recycling_container,
			       "recycling\0", channel->first_recycling,
			       NULL);
      channel->recall_id = ags_recall_id_add(channel->recall_id,
					     recall_id);
      g_object_set(G_OBJECT(recycling_container),
		   "recall_id\0", recall_id,
		   NULL);

      if(playback){
	recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(sequencer){
	recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(notation){
	recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }

      /* recycling */
      recycling = channel->first_recycling;

      for(i = 0; i < recycling_length; i++){
	recycling_container->recycling[i] = recycling;

	recycling = recycling->next;
      }
    }

    /* default recall id */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
      audio_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling_container\0", recall_id->recycling_container,
				     "recycling\0", channel->first_recycling,
				     NULL);
      audio->recall_id = ags_recall_id_add(audio->recall_id,
					   audio_recall_id);
      
      if(playback){
	audio_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(sequencer){
	audio_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(notation){
	audio_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }

      default_recall_id = audio_recall_id;
    }else{
      AgsChannel *input, *last_input;
      AgsRecycling *recycling;
      AgsRecyclingContainer *child_recycling_container;
      gint recycling_length;
      gint i;

      if((AGS_AUDIO_ASYNC & (audio->flags)) == 0){
	input = ags_channel_nth(audio->input,
				channel->line);
	last_input = input;
      }else{
	input = ags_channel_nth(audio->input,
				channel->audio_channel);
	last_input = ags_channel_pad_last(input);
      }

      /* recall id */
      audio_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling_container\0", recall_id->recycling_container,
				     "recycling\0", channel->first_recycling,
				     NULL);
      audio->recall_id = ags_recall_id_add(audio->recall_id,
					   audio_recall_id);

      if(playback){
	audio_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(sequencer){
	audio_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(notation){
	audio_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }

      /* child recycling container */
      recycling_length = ags_recycling_position(input->first_recycling, last_input->last_recycling->next,
						last_input->last_recycling);
      recycling_length++;

      child_recycling_container = (AgsRecyclingContainer *) g_object_new(AGS_TYPE_RECYCLING_CONTAINER,
									 "parent\0", audio_recall_id->recycling_container,
									 "length\0", recycling_length,
									 NULL);

      /* recall id */
      default_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling_container\0", child_recycling_container,
				       "recycling\0", input->first_recycling,
				       NULL);
      audio->recall_id = ags_recall_id_add(audio->recall_id,
					   default_recall_id);
      g_object_set(G_OBJECT(child_recycling_container),
		   "recall_id\0", default_recall_id,
		   NULL);

      if(playback){
	default_recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
      }

      if(sequencer){
	default_recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
      }

      if(notation){
	default_recall_id->flags |= AGS_RECALL_ID_NOTATION;
      }

      /* recycling */
      recycling = input->first_recycling;

      for(i = 0; i < recycling_length; i++){
	child_recycling_container->recycling[i] = recycling;

	recycling = recycling->next;
      }
    }

    if(AGS_IS_OUTPUT(channel)){
      /* follow the links */
      ags_channel_recursive_play_init_arrange_recall_id_up(channel,
							   recall_id,
							   TRUE);
      
      ags_channel_recursive_play_init_arrange_recall_id_down_input(channel,
								   default_recall_id,
								   FALSE);
    }else{
      /* follow the links */
      ags_channel_recursive_play_init_arrange_recall_id_up(channel,
							   audio_recall_id,
							   TRUE);
      
      if(channel->link != NULL){
	ags_channel_recursive_play_init_arrange_recall_id_down(channel->link,
							       recall_id,
							       FALSE);
      }
    }
  }else{
    audio_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							     recall_id->recycling_container);

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
      default_recall_id = audio_recall_id;
    }else{
      AgsChannel *input;
      GList *list;
      gint position;

      if((AGS_AUDIO_ASYNC & (audio->flags)) == 0){
	input = ags_channel_nth(audio->input,
				channel->line);
      }else{
	input = ags_channel_nth(audio->input,
				channel->audio_channel);
      }

      position = ags_recycling_container_find_child(audio_recall_id->recycling_container,
						    input->first_recycling);
      list = g_list_nth(audio_recall_id->recycling_container->children,
			position);
      default_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
								 AGS_RECYCLING_CONTAINER(list->data));
    }
  }

  /* duplicate AgsRecall templates */
  if(duplicate_templates){
    ags_channel_recursive_play_init_duplicate_up(channel,
						 recall_id);

    if(AGS_IS_OUTPUT(channel)){
      /* duplicate */
      ags_audio_duplicate_recall(audio,
				 audio_recall_id);

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	ags_audio_duplicate_recall(audio,
				   default_recall_id);
      }

      ags_channel_recursive_play_init_duplicate_down_input(channel,
							   default_recall_id);
    }else{
      /* follow the links */
      ags_channel_recursive_play_init_duplicate_down(channel->link,
						     default_recall_id);
    }
  }

  /* resolve dependencies */
  if(resolve_dependencies){
    ags_channel_recursive_play_init_resolve_up(channel,
					       recall_id);

    if(AGS_IS_OUTPUT(channel)){
      ags_audio_resolve_recall(audio,
			       audio_recall_id);

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	ags_audio_resolve_recall(audio,
				 default_recall_id);
      }

      ags_channel_recursive_play_init_resolve_down_input(channel,
							 default_recall_id);
    }else{
      /* follow the links */
      if(channel->link != NULL){
	ags_channel_recursive_play_init_resolve_down(channel->link,
						     default_recall_id);
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

  if(stage == 0){
    if(playback){
      AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[0] = recall_id;
      AGS_DEVOUT_PLAY(channel->devout_play)->flags |= AGS_DEVOUT_PLAY_PLAYBACK;
    }

    if(sequencer){
      AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[1] = recall_id;
      AGS_DEVOUT_PLAY(channel->devout_play)->flags |= AGS_DEVOUT_PLAY_SEQUENCER;
    }

    if(notation){
      AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[2] = recall_id;
      AGS_DEVOUT_PLAY(channel->devout_play)->flags |= AGS_DEVOUT_PLAY_NOTATION;
    }
  }

  for(; stage < stage_stop; stage++){
    if(AGS_IS_OUTPUT(channel)){
      ags_audio_init_recall(audio, stage,
			    audio_recall_id);
      
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	ags_audio_init_recall(audio, stage,
			      default_recall_id);
      }

      ags_channel_recursive_play_init_down_input(channel,
						 default_recall_id);
    }else{
      if(channel->link != NULL){
	ags_channel_recursive_play_init_down(channel->link,
					     default_recall_id);
      }
    }
    
    ags_channel_recursive_play_init_up(channel,
				       recall_id);
  }

  return(recall_id);
}
  
/**
 * ags_channel_cancel:
 * @channel an #AgsChannel
 * @recall_id and #AgsRecallID
 *
 * Calls for every matching @recall_id ags_recall_cancel()
 */
void
ags_channel_cancel(AgsChannel *channel,
		   AgsRecallID *recall_id)
{
  AgsRecall *recall;
  GList *list, *list_next;
  
  if(recall_id == NULL){
    return;
  }

  if(recall_id->recycling_container->parent == NULL){
    list = channel->play;
  }else{
    list = channel->recall;
  }

  while(list != NULL){
    list_next = list->next;

    recall = AGS_RECALL(list->data);

    if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0 ||
       recall->recall_id->recycling_container != recall_id->recycling_container){
      list = list_next;
      continue;
    }

    ags_recall_cancel(recall);
    
    list = list_next;
  }
}

/**
 * ags_channel_tillrecycling_cancel:
 * @channel an #AgsChannel
 * @recall_id an #AgsRecallID
 *
 * Traverses the tree down and up and calls ags_channel_cancel() for corresponding
 * @recall_id.
 */
void
ags_channel_tillrecycling_cancel(AgsChannel *channel,
				 AgsRecallID *recall_id)
{
  AgsRecallID *child_recall_id;

  auto void ags_channel_tillrecycling_cancel_input_sync(AgsChannel *input,
							AgsRecallID *input_recall_id);
  auto void ags_channel_tillrecycling_cancel_input_async(AgsChannel *input,
							 AgsRecallID *output_recall_id);
  auto void ags_channel_tillrecycling_cancel_output(AgsChannel *output,
						    AgsRecallID *output_recall_id);

  auto void ags_channel_tillrecycling_cancel_output(AgsChannel *output,
						    AgsRecallID *output_recall_id);

  void ags_channel_tillrecycling_cancel_input_sync(AgsChannel *input,
						   AgsRecallID *input_recall_id){
    AgsAudio *audio;
    AgsRecallID *child_recall_id;

    audio = AGS_AUDIO(input->audio);

    /* check if we go down */
    if(input->link != NULL){
      child_recall_id = ags_recall_id_find_recycling_container(input->recall_id,
							       recall_id->recycling_container);
      
      /* follow the links */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	ags_channel_tillrecycling_cancel_output(input->link,
						child_recall_id);
      }
    }
      
    /* call input */
    ags_channel_cancel(input,
		       input_recall_id);
  }
  void ags_channel_tillrecycling_cancel_input_async(AgsChannel *input,
						    AgsRecallID *default_recall_id){
    AgsRecallID *input_recall_id;
    gint child_position;

    /* iterate AgsInputs */
    while(input != NULL){
      input_recall_id = ags_recall_id_find_recycling_container(input->recall_id,
							       default_recall_id->recycling_container);

      /* from async to sync */
      ags_channel_tillrecycling_cancel_input_sync(input,
						  input_recall_id);
      
      /* iterate */
      input = input->next_pad;
    }
  }
  void ags_channel_tillrecycling_cancel_output(AgsChannel *output,
					       AgsRecallID *output_recall_id){
    AgsAudio *audio;
    AgsChannel *current, *input;
    AgsRecallID *default_recall_id, *input_recall_id;
    gint child_position;

    audio = AGS_AUDIO(output->audio);

    if(audio->input != NULL){
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	input = ags_channel_nth(audio->input,
				output->audio_channel);
      }else{
	input = ags_channel_nth(audio->input,
				output->line);
      }
    }
    
    /* input_recall_id - check if there is a new recycling */
    child_position = ags_recycling_container_find(output_recall_id->recycling_container,
						  input->first_recycling);
      
    if(child_position == -1){
      default_recall_id = ags_recall_id_find_recycling_container(input->recall_id,
								 output_recall_id->recycling_container);
    }else{
      GList *list;

      list = g_list_nth(ags_recycling_container_get_child_recall_id(output_recall_id->recycling_container),
			child_position);

      if(list != NULL){
	default_recall_id = ags_recall_id_find_recycling_container(channel->recall_id,
								   AGS_RECYCLING_CONTAINER(list->data));
      }else{
	default_recall_id = NULL;
      }
    }

    /* call audio */
    ags_audio_cancel(audio,
		     output_recall_id);
    
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
      ags_audio_cancel(audio,
		       default_recall_id);

      /* call input */
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	/* follow the links */
	ags_channel_tillrecycling_cancel_input_async(ags_channel_nth(audio->input, output->audio_channel),
						     output_recall_id);
      }else{      
	/* input_recall_id - check if there is a new recycling */
	child_position = ags_recycling_container_find_child(output_recall_id->recycling_container,
							    input->first_recycling);
      
	if(child_position == -1){
	  input_recall_id = ags_recall_id_find_recycling_container(input->recall_id,
								   output_recall_id->recycling_container);
	}else{
	  GList *list;

	  list = g_list_nth(output_recall_id->recycling_container->children,
			    child_position);

	  if(list != NULL){
	    input_recall_id = ags_recall_id_find_recycling_container(channel->recall_id,
								     AGS_RECYCLING_CONTAINER(list->data));
	  }else{
	    input_recall_id = NULL;
	  }
	}

	/* follow the links */
	ags_channel_tillrecycling_cancel_input_sync(input,
						    input_recall_id);
      }
    }
    
    /* call output */
    ags_channel_cancel(output,
		       output_recall_id);
  }
  void ags_channel_tillrecycling_cancel_up(AgsChannel *channel, AgsRecallID *recall_id){
    AgsAudio *audio;

    if(channel == NULL)
      return;

    audio = AGS_AUDIO(channel->audio);
    
    if(AGS_IS_INPUT(channel)){
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;

      goto ags_channel_tillrecycling_cancel_up0;
    }else{
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;
      
      goto ags_channel_tillrecycling_cancel_up1;
    }
    
    while(channel != NULL){
      audio = AGS_AUDIO(channel->audio);

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;

      /* cancel input */
      recall_id = ags_recall_id_find_recycling_container(channel->recall_id,
							 recall_id->recycling_container);

    ags_channel_tillrecycling_cancel_up0:
      ags_channel_cancel(channel,
			 recall_id);
      
      /* cancel audio */
      recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							 recall_id->recycling_container);

      ags_audio_cancel(audio,
		       recall_id);

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
	return;

      /* get output */
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0)
	channel = ags_channel_nth(audio->output,
				  channel->audio_channel);
      else
	channel = ags_channel_nth(audio->output,
				  channel->line);

      /* cancel output */
      recall_id = ags_recall_id_find_recycling_container(channel->recall_id,
							 recall_id->recycling_container);

    ags_channel_tillrecycling_cancel_up1:
      ags_channel_cancel(channel,
			 recall_id);

      channel = channel->link;
    }
  }

  /* entry point */
  if(AGS_IS_OUTPUT(channel)){
    ags_channel_tillrecycling_cancel_output(channel,
					    recall_id);
  }else{
    ags_channel_tillrecycling_cancel_input_sync(channel,
						recall_id);
  }

  ags_channel_tillrecycling_cancel_up(channel->link,
				      recall_id);
}

/**
 * ags_channel_recursive_reset_recall_ids:
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
void
ags_channel_recursive_reset_recall_ids(AgsChannel *channel, AgsChannel *link,
				       AgsChannel *old_channel_link, AgsChannel *old_link_link)
{
  GList *link_devout_play_list, *channel_devout_play_list;
  GList *link_invalid_recall_id_list, *channel_invalid_recall_id_list;
  GList *channel_recall_id_list, *link_recall_id_list;
  GList *list;
  gint stage;

  /* applying functions */
  auto void ags_audio_reset_recall_id(AgsAudio *audio,
				      GList *recall_id_list, GList *devout_play_list,
				      GList *invalid_recall_id_list);
  auto void ags_channel_reset_recall_id(AgsChannel *channel,
					GList *recall_id_list, GList *devout_play_list,
					GList *invalid_recall_id_list);

  auto void ags_channel_recursive_reset_audio_duplicate_recall(AgsAudio *audio,
							       GList *recall_id_list);
  auto void ags_channel_recursive_reset_channel_duplicate_recall(AgsChannel *channel,
								 GList *recall_id_list);

  auto void ags_channel_recursive_reset_audio_resolve_recall(AgsAudio *audio,
							     GList *recall_id_list);
  auto void ags_channel_recursive_reset_channel_resolve_recall(AgsChannel *channel,
							       GList *recall_id_list);

  auto void ags_channel_recursive_reset_audio_init_recall(AgsAudio *audio,
							  GList *recall_id_list);
  auto void ags_channel_recursive_reset_channel_init_recall(AgsChannel *channel,
							    GList *recall_id_list);

  /* collectors of AgsDevoutPlay and recall_id */
  auto GList* ags_channel_tillrecycling_collect_devout_play_down_input(AgsChannel *output,
								       GList *list, gboolean collect_recall_id);
  auto GList* ags_channel_tillrecycling_collect_devout_play_down(AgsChannel *current,
								 GList *list, gboolean collect_recall_id);
  auto GList* ags_channel_recursive_collect_devout_play_up(AgsChannel *channel,
							   gboolean collect_recall_id);

  /* tree iterator functions */
  auto void ags_channel_recursive_reset_recall_id_down_input(AgsChannel *output,
							     GList *recall_id_list, GList *devout_play_list,
							     GList *invalid_recall_id_list);
  auto void ags_channel_recursive_reset_recall_id_down(AgsChannel *current,
						       GList *recall_id_list, GList *devout_play_list,
						       GList *invalid_recall_id_list);
  auto void ags_channel_tillrecycling_reset_recall_id_up(AgsChannel *channel,
							 GList *recall_id_list, GList *devout_play_list,
							 GList *invalid_recall_id_list);

  auto void ags_channel_recursive_unset_recall_id_down_input(AgsChannel *output,
							     GList *invalid_recall_id_list);
  auto void ags_channel_recursive_unset_recall_id_down(AgsChannel *current,
						       GList *invalid_recall_id_list);
  auto void ags_channel_tillrecycling_unset_recall_id_up(AgsChannel *channel,
							 GList *invalid_recall_id_list);

  auto void ags_channel_recursive_duplicate_recall_down_input(AgsChannel *output,
							      GList *recall_id_list);
  auto void ags_channel_recursive_duplicate_recall_down(AgsChannel *channel,
							GList *recall_id_list);
  auto void ags_channel_tillrecycling_duplicate_recall_up(AgsChannel *channel,
							  GList *recall_id_list);

  auto void ags_channel_recursive_resolve_recall_down_input(AgsChannel *output,
							    GList *recall_id);
  auto void ags_channel_recursive_resolve_recall_down(AgsChannel *channel,
						      GList *recall_id);
  auto void ags_channel_tillrecycling_resolve_recall_up(AgsChannel *channel,
							GList *recall_id);

  auto void ags_channel_recursive_init_recall_down_input(AgsChannel *output,
							 GList *recall_id);
  auto void ags_channel_recursive_init_recall_down(AgsChannel *channel,
						   GList *recall_id);
  auto void ags_channel_tillrecycling_init_recall_up(AgsChannel *channel,
						     GList *recall_id);

  /* implementation */
  void ags_audio_reset_recall_id(AgsAudio *audio,
				 GList *recall_id_list, GList *devout_play_list,
				 GList *invalid_recall_id_list){
    AgsChannel *channel;
    AgsRecall *recall;
    AgsRecallID *recall_id;
    AgsDevoutPlay *devout_play;
    GList *list;
    gboolean play;
    gboolean playback, sequencer, notation;

    AgsRecycling *recycling;
    gint recycling_length;
    gint i;

    while(invalid_recall_id_list != NULL){
      recall_id = AGS_RECALL_ID(invalid_recall_id_list->data);
      
      play = (recall_id->recycling_container->parent == NULL) ? TRUE: FALSE;
      list = play ? audio->play: audio->recall;

      /* remove AgsRecalls */
      while((list = ags_recall_find_recycling_container(list, G_OBJECT(recall_id->recycling_container))) != NULL){
	recall = AGS_RECALL(list->data);

#ifdef AGS_DEBUG
	g_message("reset recall id: invalidated\0");
#endif
	
	ags_audio_remove_recall(audio,
				(GObject *) recall,
				play);
	
	list = list->next;
      }
      
      /* remove AgsRecallID */
      ags_audio_remove_recall_id(audio, (GObject *) recall_id);

      /* iterate */
      invalid_recall_id_list = invalid_recall_id_list->next;
    }
    
    while(recall_id_list != NULL){
      /* playback */
      devout_play = AGS_DEVOUT_PLAY(devout_play_list->data);
      playback = ((AGS_DEVOUT_PLAY_PLAYBACK & (devout_play->flags)) != 0) ? TRUE: FALSE;

      if(playback){
#ifdef AGS_DEBUG
	g_message("recall id reset: play\0");
#endif

	/* append new recall id */
	recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				 "recycling\0", AGS_RECALL_ID(recall_id_list->data)->recycling,
				 "recycling_container\0", AGS_RECALL_ID(recall_id_list->data)->recycling_container,
				 NULL);
	audio->recall_id = ags_recall_id_add(audio->recall_id,
					     recall_id);

	if((AGS_RECALL_ID_PLAYBACK & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	  recall_id->flags |= AGS_RECALL_ID_PLAYBACK;
	}

	if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	  recall_id->flags |= AGS_RECALL_ID_SEQUENCER;
	}

	if((AGS_RECALL_ID_NOTATION & (AGS_RECALL_ID(recall_id_list->data)->flags)) != 0){
	  recall_id->flags |= AGS_RECALL_ID_NOTATION;
	}
	
	/* iterate */
	recall_id_list = recall_id_list->next;
	devout_play_list = devout_play_list->next;
      }

      if(recall_id_list == NULL){
	break;
      }

      /* sequencer */
      devout_play = AGS_DEVOUT_PLAY(devout_play_list->data);
      sequencer = ((AGS_DEVOUT_PLAY_SEQUENCER & (devout_play->flags)) != 0) ? TRUE: FALSE;      

      if(sequencer){
#ifdef AGS_DEBUG
	g_message("recall id reset: sequencer\0");
#endif

	/* append new recall id */
	recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				 "recycling\0", AGS_RECALL_ID(recall_id_list->data)->recycling,
				 "recycling_container\0", AGS_RECALL_ID(recall_id_list->data)->recycling_container,
				 NULL);
	audio->recall_id = ags_recall_id_add(audio->recall_id,
					     recall_id);

	/* iterate */
	recall_id_list = recall_id_list->next;
	devout_play_list = devout_play_list->next;

      }

      if(recall_id_list == NULL){
	break;
      }

      /* notation */
      devout_play = AGS_DEVOUT_PLAY(devout_play_list->data);
      notation = ((AGS_DEVOUT_PLAY_NOTATION & (devout_play->flags)) != 0) ? TRUE: FALSE;

      if(notation){
#ifdef AGS_DEBUG
	g_message("recall id reset: notation\0");
#endif

	/* append new recall id */
	recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				 "recycling\0", AGS_RECALL_ID(recall_id_list->data)->recycling,
				 "recycling_container\0", AGS_RECALL_ID(recall_id_list->data)->recycling_container,
				 NULL);
	audio->recall_id = ags_recall_id_add(audio->recall_id,
					     recall_id);

	/* iterate */
	recall_id_list = recall_id_list->next;
	devout_play_list = devout_play_list->next;
      }
    }
  }
  void ags_channel_reset_recall_id(AgsChannel *channel,
				   GList *recall_id_list, GList *devout_play_list,
				   GList *invalid_recall_id_list){
    AgsRecall *recall;
    AgsRecallID *recall_id;
    AgsDevoutPlay *devout_play;
    GList *list;
    gboolean play;
    gboolean playback, sequencer, notation;

    while(invalid_recall_id_list != NULL){
      recall_id = AGS_RECALL_ID(invalid_recall_id_list->data);
      
      play = (recall_id->recycling_container->parent == NULL) ? TRUE: FALSE;
      list = play ? channel->play: channel->recall;

      /* unref AgsRecalls */
      while((list = ags_recall_find_recycling_container(list, G_OBJECT(recall_id->recycling_container))) != NULL){
	recall = AGS_RECALL(list->data);
	
	ags_channel_remove_recall(channel,
				  (GObject *) recall,
				  play);
	
	list = list->next;
      }
      
      /* unref AgsRecallID */
      ags_channel_remove_recall_id(channel,
				   recall_id);

      /* iterate */
      invalid_recall_id_list = invalid_recall_id_list->next;
    }
    
    while(recall_id_list != NULL){
      /* playback */
      devout_play = AGS_DEVOUT_PLAY(devout_play_list->data);
      playback = (((AGS_DEVOUT_PLAY_PLAYBACK & (devout_play->flags)) != 0) ? TRUE: FALSE);

      if(playback){
#ifdef AGS_DEBUG
	g_message("recall id reset: playback\0");
#endif

	/* append new recall id */
	recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				 "recycling\0", AGS_RECALL_ID(recall_id_list->data)->recycling,
				 "recycling_container\0", AGS_RECALL_ID(recall_id_list->data)->recycling_container,
				 NULL);
	channel->recall_id = ags_recall_id_add(channel->recall_id,
					       recall_id);
	recall_id->flags |= AGS_RECALL_ID_PLAYBACK;

	/* iterate */
	recall_id_list = recall_id_list->next;
	devout_play_list = devout_play_list->next;
      }

      if(recall_id_list == NULL){
	break;
      }

      /* sequencer */
      devout_play = AGS_DEVOUT_PLAY(devout_play_list->data);
      sequencer = (((AGS_DEVOUT_PLAY_SEQUENCER & (devout_play->flags)) != 0) ? TRUE: FALSE);

      if(sequencer){
#ifdef AGS_DEBUG
	g_message("recall id reset: sequencer\0");
#endif

	/* append new recall id */
	recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				 "recycling\0", AGS_RECALL_ID(recall_id_list->data)->recycling,
				 "recycling_container\0", AGS_RECALL_ID(recall_id_list->data)->recycling_container,
				 NULL);
	channel->recall_id = ags_recall_id_add(channel->recall_id,
					       recall_id);
	recall_id->flags |= AGS_RECALL_ID_SEQUENCER;

	/* iterate */
	recall_id_list = recall_id_list->next;
	devout_play_list = devout_play_list->next;
      }

      if(recall_id_list == NULL){
	break;
      }

      /* notation */
      devout_play = AGS_DEVOUT_PLAY(devout_play_list->data);
      notation = (((AGS_DEVOUT_PLAY_NOTATION & (devout_play->flags)) != 0) ? TRUE: FALSE);

      if(notation){
#ifdef AGS_DEBUG
	g_message("recall id reset: notation\0");
#endif

	/* append new recall id */
	recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				 "recycling\0", AGS_RECALL_ID(recall_id_list->data)->recycling,
				 "recycling_container\0", AGS_RECALL_ID(recall_id_list->data)->recycling_container,
				 NULL);
	channel->recall_id = ags_recall_id_add(channel->recall_id,
					       recall_id);
	recall_id->flags |= AGS_RECALL_ID_NOTATION;
	
	/* iterate */
	recall_id_list = recall_id_list->next;
	devout_play_list = devout_play_list->next;
      }
    }
  }

  void ags_channel_recursive_reset_audio_duplicate_recall(AgsAudio *audio,
							  GList *recall_id_list){
    AgsRecallID *recall_id;

    while(recall_id_list != NULL){
      recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							 AGS_RECALL_ID(recall_id_list->data)->recycling_container);

      ags_audio_duplicate_recall(audio,
				 recall_id);

      recall_id_list = recall_id_list->next;
    }
  }
  void ags_channel_recursive_reset_channel_duplicate_recall(AgsChannel *channel,
							    GList *recall_id_list){
    AgsRecallID *recall_id;

    while(recall_id_list != NULL){
      recall_id = ags_recall_id_find_recycling_container(channel->recall_id,
							 AGS_RECALL_ID(recall_id_list->data)->recycling_container);

      ags_channel_duplicate_recall(channel,
				   recall_id);
      
      recall_id_list = recall_id_list->next;
    }
  }

  void ags_channel_recursive_reset_audio_resolve_recall(AgsAudio *audio,
							GList *recall_id_list){
    AgsRecallID *recall_id;

    while(recall_id_list != NULL){
      recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							 AGS_RECALL_ID(recall_id_list->data)->recycling_container);

      ags_audio_resolve_recall(audio,
			       recall_id);

      recall_id_list = recall_id_list->next;
    }
  }
  void ags_channel_recursive_reset_channel_resolve_recall(AgsChannel *channel,
							  GList *recall_id_list){
    AgsRecallID *recall_id;

    while(recall_id_list != NULL){
      recall_id = ags_recall_id_find_recycling_container(channel->recall_id,
							 AGS_RECALL_ID(recall_id_list->data)->recycling_container);

      ags_channel_resolve_recall(channel,
				 recall_id);

      recall_id_list = recall_id_list->next;
    }
  }

  void ags_channel_recursive_reset_audio_init_recall(AgsAudio *audio,
						     GList *recall_id_list){
    AgsRecallID *recall_id;

    while(recall_id_list != NULL){
      recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							 AGS_RECALL_ID(recall_id_list->data)->recycling_container);

      ags_audio_init_recall(audio, stage,
			    recall_id);

      recall_id_list = recall_id_list->next;
    } 
  }
  void ags_channel_recursive_reset_channel_init_recall(AgsChannel *channel,
						       GList *recall_id_list){
    AgsAudio *audio;
    AgsRecallID *recall_id;

    while(recall_id_list != NULL){
      recall_id = ags_recall_id_find_recycling_container(channel->recall_id,
							 AGS_RECALL_ID(recall_id_list->data)->recycling_container);

      ags_channel_init_recall(channel, stage,
			      recall_id);

      recall_id_list = recall_id_list->next;
    } 
  }
  GList* ags_channel_tillrecycling_collect_devout_play_down_input(AgsChannel *output,
								  GList *list, gboolean collect_recall_id){
    AgsAudio *audio;
    AgsChannel *current;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      return(list);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input,
				output->audio_channel);

      while(current != NULL){
	/* collect recall id and the recalls purpose */
	if(current->devout_play != NULL){
	  if((AGS_DEVOUT_PLAY_PLAYBACK & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 &&
	     AGS_DEVOUT_PLAY(current->devout_play)->recall_id[0] != NULL){
#ifdef AGS_DEBUG
	    g_message("recall id collect: play\0");
#endif

	    list = g_list_prepend(list,
				  (collect_recall_id ? AGS_DEVOUT_PLAY(current->devout_play)->recall_id[0]: current->devout_play));
	  }

	  if((AGS_DEVOUT_PLAY_SEQUENCER & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 &&
	     AGS_DEVOUT_PLAY(current->devout_play)->recall_id[1] != NULL){
#ifdef AGS_DEBUG
	    g_message("recall id collect: sequencer\0");
#endif

	    list = g_list_prepend(list,
				  (collect_recall_id ? AGS_DEVOUT_PLAY(current->devout_play)->recall_id[1]: current->devout_play));
	  }

	  if((AGS_DEVOUT_PLAY_NOTATION & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 &&
	     AGS_DEVOUT_PLAY(current->devout_play)->recall_id[2] != NULL){
#ifdef AGS_DEBUG
	    g_message("recall id collect: notation\0");
#endif

	    list = g_list_prepend(list,
				  (collect_recall_id ? AGS_DEVOUT_PLAY(current->devout_play)->recall_id[2]: current->devout_play));
	  }
	}

	/* follow the links */
	list = ags_channel_tillrecycling_collect_devout_play_down(current->link,
								  list, collect_recall_id);

	current = current->next_pad;
      }
    }else{
      current = ags_channel_nth(audio->input, output->line);
      
      /* collect recall id and the recalls purpose */
      if(current->devout_play != NULL){
	if((AGS_DEVOUT_PLAY_PLAYBACK & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 &&
	   AGS_DEVOUT_PLAY(current->devout_play)->recall_id[0] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: play\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_DEVOUT_PLAY(current->devout_play)->recall_id[0]: current->devout_play));
	}

	if((AGS_DEVOUT_PLAY_SEQUENCER & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 &&
	   AGS_DEVOUT_PLAY(current->devout_play)->recall_id[1] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: sequencer\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_DEVOUT_PLAY(current->devout_play)->recall_id[1]: current->devout_play));
	}

	if((AGS_DEVOUT_PLAY_NOTATION & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 &&
	   AGS_DEVOUT_PLAY(current->devout_play)->recall_id[2] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: notation\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_DEVOUT_PLAY(current->devout_play)->recall_id[2]: current->devout_play));
	}
      }

      /* follow the links */
      list = ags_channel_tillrecycling_collect_devout_play_down(current->link,
								list, collect_recall_id);
    }

    return(list);
  }
  GList* ags_channel_tillrecycling_collect_devout_play_down(AgsChannel *current,
							    GList *list, gboolean collect_recall_id){
    AgsAudio *audio;

    if(current == NULL)
      return(list);

    audio = AGS_AUDIO(current->audio);

    /* collect devout play */
    if(current->devout_play != NULL){
      if((AGS_DEVOUT_PLAY_PLAYBACK & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 &&
	 AGS_DEVOUT_PLAY(current->devout_play)->recall_id[0] != NULL){
#ifdef AGS_DEBUG
	g_message("recall id collect: play\0");
#endif

	list = g_list_prepend(list,
			      (collect_recall_id ? AGS_DEVOUT_PLAY(current->devout_play)->recall_id[0]: current->devout_play));
      }

      if((AGS_DEVOUT_PLAY_SEQUENCER & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 &&
	 AGS_DEVOUT_PLAY(current->devout_play)->recall_id[1] != NULL){
#ifdef AGS_DEBUG
	g_message("recall id collect: sequencer - a\0");
#endif

	list = g_list_prepend(list,
			      (collect_recall_id ? AGS_DEVOUT_PLAY(current->devout_play)->recall_id[1]: current->devout_play));
      }

      if((AGS_DEVOUT_PLAY_NOTATION & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 &&
	 AGS_DEVOUT_PLAY(current->devout_play)->recall_id[2] != NULL){
#ifdef AGS_DEBUG
	g_message("recall id collect: notation\0");
#endif

	list = g_list_prepend(list,
			      (collect_recall_id ? AGS_DEVOUT_PLAY(current->devout_play)->recall_id[2]: current->devout_play));
      }
    }
    
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
      return(list);

    /* follow the links */
    list = ags_channel_tillrecycling_collect_devout_play_down_input(current,
								    list, collect_recall_id);

    return(list);
  }
  GList* ags_channel_recursive_collect_devout_play_up(AgsChannel *channel,
						      gboolean collect_recall_id){
    AgsAudio *audio;
    AgsChannel *current;
    GList *list;

    if(channel == NULL){
      return(NULL);
    }

    current = channel;
    list = NULL;

    if(AGS_IS_OUTPUT(channel)){
      audio = AGS_AUDIO(current->audio);

      goto ags_channel_recursive_collect_devout_play_upOUTPUT;
    }

    while(current != NULL){
      audio = AGS_AUDIO(current->audio);

      /* input */
      if(current->devout_play != NULL){
	if((AGS_DEVOUT_PLAY_PLAYBACK & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 &&
	   AGS_DEVOUT_PLAY(current->devout_play)->recall_id[0] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: play\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_DEVOUT_PLAY(current->devout_play)->recall_id[0]: current->devout_play));
	}

	if((AGS_DEVOUT_PLAY_SEQUENCER & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 &&
	   AGS_DEVOUT_PLAY(current->devout_play)->recall_id[1] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: sequencer\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_DEVOUT_PLAY(current->devout_play)->recall_id[1]: current->devout_play));
	}

	if((AGS_DEVOUT_PLAY_NOTATION & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 &&
	   AGS_DEVOUT_PLAY(current->devout_play)->recall_id[2] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: notation\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_DEVOUT_PLAY(current->devout_play)->recall_id[2]: current->devout_play));
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
      if(current->devout_play != NULL){
	if((AGS_DEVOUT_PLAY_PLAYBACK & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 &&
	   AGS_DEVOUT_PLAY(current->devout_play)->recall_id[0] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: play\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_DEVOUT_PLAY(current->devout_play)->recall_id[0]: current->devout_play));
	}

	if((AGS_DEVOUT_PLAY_SEQUENCER & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 &&
	   AGS_DEVOUT_PLAY(current->devout_play)->recall_id[1] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: sequencer\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_DEVOUT_PLAY(current->devout_play)->recall_id[1]: current->devout_play));
	}

	if((AGS_DEVOUT_PLAY_NOTATION & (AGS_DEVOUT_PLAY(current->devout_play)->flags)) != 0 &&
	   AGS_DEVOUT_PLAY(current->devout_play)->recall_id[2] != NULL){
#ifdef AGS_DEBUG
	  g_message("recall id collect: notation\0");
#endif

	  list = g_list_prepend(list,
				(collect_recall_id ? AGS_DEVOUT_PLAY(current->devout_play)->recall_id[2]: current->devout_play));
	}
      }

      current = current->link;
    }

    return(list);
  }
  void ags_channel_recursive_reset_recall_id_down_input(AgsChannel *output,
							GList *recall_id_list, GList *devout_play_list,
							GList *invalid_recall_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;
    GList *next_recall_id_list;
    GList *next_invalid_recall_id_list;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input, output->audio_channel);

      while(current != NULL){
	/* reset AgsInput */
	ags_channel_reset_recall_id(current,
				   recall_id_list, devout_play_list,
				   invalid_recall_id_list);

	/* retrieve next recall id list */
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	  next_recall_id_list = recall_id_list;
	  next_invalid_recall_id_list = invalid_recall_id_list;
	}else{
	  next_recall_id_list = NULL;

	  while(recall_id_list != NULL){
	    next_recall_id_list = g_list_concat(next_recall_id_list,
						ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(recall_id_list)->recycling_container));

	    recall_id_list = recall_id_list->next;
	  }

	  next_invalid_recall_id_list = NULL;

	  while(invalid_recall_id_list != NULL){
	    next_invalid_recall_id_list = g_list_concat(next_invalid_recall_id_list,
							ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list)->recycling_container));

	    invalid_recall_id_list = invalid_recall_id_list->next;
	  }
	}

	/* follow the links */
	ags_channel_recursive_reset_recall_id_down(current->link,
						   next_recall_id_list, devout_play_list,
						   next_invalid_recall_id_list);

	/* free allocated lists */
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	  g_list_free(next_recall_id_list);
	  g_list_free(next_invalid_recall_id_list);
	}

	current = current->next_pad;
      }
    }else{
      current = ags_channel_nth(audio->input,
				output->line);

      /* reset AgsInput */
      ags_channel_reset_recall_id(current,
				 recall_id_list, devout_play_list,
				 invalid_recall_id_list);

      /* retrieve next recall id list */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	next_recall_id_list = recall_id_list;
	next_invalid_recall_id_list = invalid_recall_id_list;
      }else{
	next_recall_id_list = NULL;

	while(recall_id_list != NULL){
	  next_recall_id_list = g_list_concat(next_recall_id_list,
					      ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(recall_id_list)->recycling_container));

	  recall_id_list = recall_id_list->next;
	}

	next_invalid_recall_id_list = NULL;

	while(invalid_recall_id_list != NULL){
	  next_invalid_recall_id_list = g_list_concat(next_invalid_recall_id_list,
						      ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list)->recycling_container));

	  invalid_recall_id_list = invalid_recall_id_list->next;
	}
      }

      /* follow the links */
      ags_channel_recursive_reset_recall_id_down(current->link,
						recall_id_list, devout_play_list,
						next_invalid_recall_id_list);

      /* free allocated lists */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	g_list_free(next_recall_id_list);
	g_list_free(next_invalid_recall_id_list);
      }
    }
  }
  void ags_channel_recursive_reset_recall_id_down(AgsChannel *current,
						  GList *recall_id_list, GList *devout_play_list,
						  GList *invalid_recall_id_list)
  {
    AgsAudio *audio;
    GList *next_recall_id_list;
    GList *next_invalid_recall_id_list;

    if(current == NULL)
      return;

    audio = AGS_AUDIO(current->audio);
    
    /* reset on AgsOutput */
    ags_channel_reset_recall_id(current,
				recall_id_list, devout_play_list,
				invalid_recall_id_list);

    /* retrieve next recall id list */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      next_recall_id_list = recall_id_list;
      next_invalid_recall_id_list = invalid_recall_id_list;
    }else{
      next_recall_id_list = NULL;

      while(recall_id_list != NULL){
	next_recall_id_list = g_list_concat(next_recall_id_list,
					    ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(recall_id_list)->recycling_container));

	recall_id_list = recall_id_list->next;
      }

      next_invalid_recall_id_list = NULL;

      while(invalid_recall_id_list != NULL){
	next_invalid_recall_id_list = g_list_concat(next_invalid_recall_id_list,
						    ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list)->recycling_container));

	invalid_recall_id_list = invalid_recall_id_list->next;
      }
    }

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      /* reset on AgsAudio */
      ags_audio_reset_recall_id(audio,
				recall_id_list, devout_play_list,
				invalid_recall_id_list);
      
      /* reset new recall id on AgsAudio */
      ags_audio_reset_recall_id(audio,
				next_recall_id_list, devout_play_list,
				next_invalid_recall_id_list);
    }else{
      /* reset on AgsAudio */
      ags_audio_reset_recall_id(audio,
				recall_id_list, devout_play_list,
				invalid_recall_id_list);
    }

    /* follow the links */
    ags_channel_recursive_reset_recall_id_down_input(current,
						     recall_id_list, devout_play_list,
						     next_invalid_recall_id_list);
    
    /* free allocated lists */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      g_list_free(next_recall_id_list);
      g_list_free(next_invalid_recall_id_list);
    }
  }
  void ags_channel_tillrecycling_reset_recall_id_up(AgsChannel *channel,
						    GList *recall_id_list, GList *devout_play_list,
						    GList *invalid_recall_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL)
      return;

    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      audio = AGS_AUDIO(current->audio);

      goto ags_channel_tillrecycling_reset_recall_id_upOUTPUT;
    }
    
    while(current != NULL){
      audio = AGS_AUDIO(current->audio);

      /* input */
      ags_channel_reset_recall_id(current,
				  recall_id_list, devout_play_list,
				  invalid_recall_id_list);
      

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }      

      /* audio */
      ags_audio_reset_recall_id(audio,
				recall_id_list, devout_play_list,
				invalid_recall_id_list);

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

    ags_channel_tillrecycling_reset_recall_id_upOUTPUT:
      ags_channel_reset_recall_id(current,
				  recall_id_list, devout_play_list,
				  invalid_recall_id_list);

      ags_audio_reset_recall_id(audio,
				recall_id_list, devout_play_list,
				invalid_recall_id_list);

      current = current->link;
    }
  }
  
  void ags_channel_recursive_unset_recall_id_down_input(AgsChannel *output,
							GList *invalid_recall_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;
    GList *next_invalid_recall_id_list;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input,
				output->audio_channel);

      while(current != NULL){
	/* reset AgsInput */
	ags_channel_reset_recall_id(current,
				   NULL, NULL,
				   invalid_recall_id_list);

	/* retrieve next recall id list */
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	  next_invalid_recall_id_list = invalid_recall_id_list;
	}else{
	  next_invalid_recall_id_list = NULL;

	  while(invalid_recall_id_list != NULL){
	    next_invalid_recall_id_list = g_list_concat(next_invalid_recall_id_list,
							ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list)->recycling_container));

	    invalid_recall_id_list = invalid_recall_id_list->next;
	  }
	}
    
	/* follow the links */
	ags_channel_recursive_unset_recall_id_down(current->link,
						   next_invalid_recall_id_list);

	/* free allocated lists */
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  g_list_free(next_invalid_recall_id_list);
	}

	current = current->next_pad;
      }
    }else{
      current = ags_channel_nth(audio->input,
				output->line);

      /* reset AgsInput */
      ags_channel_reset_recall_id(current,
				 NULL, NULL,
				 invalid_recall_id_list);

      /* retrieve next recall id list */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	next_invalid_recall_id_list = invalid_recall_id_list;
      }else{
	next_invalid_recall_id_list = NULL;

	while(invalid_recall_id_list != NULL){
	  next_invalid_recall_id_list = g_list_concat(next_invalid_recall_id_list,
						      ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list)->recycling_container));

	  invalid_recall_id_list = invalid_recall_id_list->next;
	}
      }
      
      /* follow the links */
      ags_channel_recursive_unset_recall_id_down(current->link,
						next_invalid_recall_id_list);

      /* free allocated lists */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	g_list_free(next_invalid_recall_id_list);
      }
    }
  }
  void ags_channel_recursive_unset_recall_id_down(AgsChannel *current,
						 GList *invalid_recall_id_list)
  {
    AgsAudio *audio;
    GList *next_invalid_recall_id_list;

    if(current == NULL)
      return;

    audio = AGS_AUDIO(current->audio);
    
    /* reset on AgsOutput */
    ags_channel_reset_recall_id(current,
			       NULL, NULL,
			       invalid_recall_id_list);

    /* retrieve next recall id list */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      next_invalid_recall_id_list = invalid_recall_id_list;
    }else{
      next_invalid_recall_id_list = NULL;

      while(invalid_recall_id_list != NULL){
	next_invalid_recall_id_list = g_list_concat(next_invalid_recall_id_list,
						    ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(invalid_recall_id_list)->recycling_container));

	invalid_recall_id_list = invalid_recall_id_list->next;
      }
    }

    /* reset on AgsAudio */
    ags_audio_reset_recall_id(audio,
			     NULL, NULL,
			     invalid_recall_id_list);

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      ags_audio_reset_recall_id(audio,
			       NULL, NULL,
			       invalid_recall_id_list);

      ags_audio_reset_recall_id(audio,
			       NULL, NULL,
			       next_invalid_recall_id_list);
    }else{
      ags_audio_reset_recall_id(audio,
			       NULL, NULL,
			       invalid_recall_id_list);
    }
    
    /* go down */
    ags_channel_recursive_unset_recall_id_down_input(current,
						    next_invalid_recall_id_list);

    /* free allocated lists */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      g_list_free(next_invalid_recall_id_list);
    }
  }
  void ags_channel_tillrecycling_unset_recall_id_up(AgsChannel *channel,
						   GList *invalid_recall_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL)
      return;

    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      audio = AGS_AUDIO(current->audio);

      goto ags_channel_tillrecycling_reset_recall_id_upOUTPUT;
    }
    
    while(current != NULL){
      audio = AGS_AUDIO(current->audio);

      /* input */
      ags_channel_reset_recall_id(current,
				 NULL, NULL,
				 invalid_recall_id_list);


      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }      

      /* audio */
      ags_audio_reset_recall_id(audio,
			       NULL, NULL,
			       invalid_recall_id_list);

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

    ags_channel_tillrecycling_reset_recall_id_upOUTPUT:

      ags_audio_reset_recall_id(audio,
			       NULL, NULL,
			       invalid_recall_id_list);


      ags_channel_reset_recall_id(current,
				 NULL, NULL,
				 invalid_recall_id_list);

      current = current->link;
    }
  }

  void ags_channel_recursive_duplicate_recall_down_input(AgsChannel *output,
							 GList *recall_id_list){

    AgsAudio *audio;
    AgsChannel *current;
    GList *next_recall_id_list;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input,
				output->audio_channel);

      while(current != NULL){
	/* duplicate input */
	ags_channel_recursive_reset_channel_duplicate_recall(current,
							     recall_id_list);

	if(current->link != NULL){
	  /* retrieve next recall id list */
	  if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	    next_recall_id_list = recall_id_list;
	  }else{
	    next_recall_id_list = NULL;

	    while(recall_id_list != NULL){
	      next_recall_id_list = g_list_concat(next_recall_id_list,
						  ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(recall_id_list)->recycling_container));

	      recall_id_list = recall_id_list->next;
	    }
	  }

	  /* follow the links */
	  ags_channel_recursive_duplicate_recall_down(current->link,
						      next_recall_id_list);

	  if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	    g_list_free(next_recall_id_list);
	  }
	}

	current = current->next_pad;
      }
    }else{
      current = ags_channel_nth(audio->input,
				output->line);

      /* duplicate input */
      ags_channel_recursive_reset_channel_duplicate_recall(current,
							   recall_id_list);

      /* retrieve next recall id list */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	next_recall_id_list = recall_id_list;
      }else{
	next_recall_id_list = NULL;

	while(recall_id_list != NULL){
	  next_recall_id_list = g_list_concat(next_recall_id_list,
					      ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(recall_id_list)->recycling_container));

	  recall_id_list = recall_id_list->next;
	}
      }

      /* follow the links */
      ags_channel_recursive_duplicate_recall_down(current->link,
						  next_recall_id_list);

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	g_list_free(next_recall_id_list);
      }
    }
  }
  void ags_channel_recursive_duplicate_recall_down(AgsChannel *current,
						   GList *recall_id_list){

    AgsAudio *audio;
    GList *next_recall_id_list;

    if(current == NULL)
      return;

    audio = AGS_AUDIO(current->audio);

    /* duplicate output */
    ags_channel_recursive_reset_channel_duplicate_recall(current,
							 recall_id_list);
    
    /* retrieve next recall id list */
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
      next_recall_id_list = recall_id_list;
    }else{
      next_recall_id_list = NULL;

      while(recall_id_list != NULL){
	next_recall_id_list = g_list_concat(next_recall_id_list,
					    ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(recall_id_list)->recycling_container));

	recall_id_list = recall_id_list->next;
      }
    }
        
    /* duplicate audio */
    ags_channel_recursive_reset_audio_duplicate_recall(audio,
						       recall_id_list);
    
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
      ags_channel_recursive_reset_audio_duplicate_recall(audio,
							 next_recall_id_list);
    }

    /* go down */
    ags_channel_recursive_duplicate_recall_down_input(current,
						      next_recall_id_list);

    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
      g_list_free(next_recall_id_list);
    }
  }
  void ags_channel_tillrecycling_duplicate_recall_up(AgsChannel *channel,
						     GList *recall_id_list){
    AgsAudio *audio;
    AgsChannel *current;

    if(channel == NULL)
      return;

    audio = AGS_AUDIO(channel->audio);
    current = channel;

    if(AGS_IS_OUTPUT(channel)){
      goto ags_channel_tillrecycling_duplicate_recall_upOUTPUT;
    }

    while(current != NULL){
      audio = AGS_AUDIO(current->audio);

      /* duplicate input */
      ags_channel_recursive_reset_channel_duplicate_recall(current,
							   recall_id_list);
      

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      /* duplicate audio */
      ags_channel_recursive_reset_audio_duplicate_recall(audio,
							 recall_id_list);


      /* duplicate output */
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

    ags_channel_tillrecycling_duplicate_recall_upOUTPUT:

      ags_channel_recursive_reset_channel_duplicate_recall(current,
							   recall_id_list);


      current = current->link;
    }
  }

  void ags_channel_recursive_resolve_recall_down_input(AgsChannel *output,
						       GList *recall_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;
    GList *next_recall_id_list;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input,
				output->audio_channel);

      while(current != NULL){
	/* resolve input */
	ags_channel_recursive_reset_channel_resolve_recall(current,
							   recall_id_list);

	if(current->link != NULL){
	  /* retrieve next recall id list */
	  if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	    next_recall_id_list = recall_id_list;
	  }else{
	    next_recall_id_list = NULL;

	    while(recall_id_list != NULL){
	      next_recall_id_list = g_list_concat(next_recall_id_list,
						  ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(recall_id_list)->recycling_container));

	      recall_id_list = recall_id_list->next;
	    }
	  }

	  /* follow the links */
	  ags_channel_recursive_resolve_recall_down(current->link,
						    next_recall_id_list);

	  if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	    g_list_free(next_recall_id_list);
	  }
	}

	current = current->next_pad;
      }
    }else{
      current = ags_channel_nth(audio->input,
				output->line);

      /* resolve input */
      ags_channel_recursive_reset_channel_resolve_recall(current,
							 recall_id_list);

      /* retrieve next recall id list */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	next_recall_id_list = recall_id_list;
      }else{
	next_recall_id_list = NULL;

	while(recall_id_list != NULL){
	  next_recall_id_list = g_list_concat(next_recall_id_list,
					      ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(recall_id_list)->recycling_container));

	  recall_id_list = recall_id_list->next;
	}
      }

      /* follow the links */
      ags_channel_recursive_resolve_recall_down(current->link,
						next_recall_id_list);

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	g_list_free(next_recall_id_list);
      }
    }
  }
  void ags_channel_recursive_resolve_recall_down(AgsChannel *current,
						 GList *recall_id_list)
  {
    AgsAudio *audio;
    GList *next_recall_id_list;

    if(current == NULL)
      return;

    audio = AGS_AUDIO(current->audio);

    /* resolve output */
    ags_channel_recursive_reset_channel_resolve_recall(current,
						       recall_id_list);
    
    /* retrieve next recall id list */
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
      next_recall_id_list = recall_id_list;
    }else{
      next_recall_id_list = NULL;

      while(recall_id_list != NULL){
	next_recall_id_list = g_list_concat(next_recall_id_list,
					    ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(recall_id_list)->recycling_container));

	recall_id_list = recall_id_list->next;
      }
    }
        
    /* resolve audio */
    ags_channel_recursive_reset_audio_resolve_recall(audio,
						     recall_id_list);
    
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
      ags_channel_recursive_reset_audio_resolve_recall(audio,
						       next_recall_id_list);
    }

    /* go down */
    ags_channel_recursive_resolve_recall_down_input(current,
						    next_recall_id_list);

    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
      g_list_free(next_recall_id_list);
    }
  }
  void ags_channel_tillrecycling_resolve_recall_up(AgsChannel *channel,
						   GList *recall_id_list)
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
							 recall_id_list);
      

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      /* resolve audio */
      ags_channel_recursive_reset_audio_resolve_recall(audio,
						       recall_id_list);


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
							 recall_id_list);


      current = current->link;
    }
  }
  void ags_channel_recursive_init_recall_down_input(AgsChannel *output,
						    GList *recall_id_list)
  {
    AgsAudio *audio;
    AgsChannel *current;
    GList *next_recall_id_list;

    audio = AGS_AUDIO(output->audio);

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      current = ags_channel_nth(audio->input, output->audio_channel);

      while(current != NULL){
	/* retrieve next recall id list */
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	  next_recall_id_list = recall_id_list;
	}else{
	  next_recall_id_list = NULL;

	  while(recall_id_list != NULL){
	    next_recall_id_list = g_list_concat(next_recall_id_list,
						ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(recall_id_list)->recycling_container));

	    recall_id_list = recall_id_list->next;
	  }
	}

	/* follow the links */
	ags_channel_recursive_init_recall_down(current->link,
					       next_recall_id_list);

	/* init AgsInput */
	ags_channel_recursive_reset_channel_init_recall(current,
							recall_id_list);

	/* free allocated lists */
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  g_list_free(next_recall_id_list);
	}

	current = current->next_pad;
      }
    }else{
      current = ags_channel_nth(audio->input, output->line);

      /* retrieve next recall id list */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
	next_recall_id_list = recall_id_list;
      }else{
	next_recall_id_list = NULL;

	while(recall_id_list != NULL){
	  next_recall_id_list = g_list_concat(next_recall_id_list,
					      ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(recall_id_list)->recycling_container));

	  recall_id_list = recall_id_list->next;
	}
      }
      
      /* follow the links */
      ags_channel_recursive_init_recall_down(current->link,
					     next_recall_id_list);

      /* init AgsInput */
      ags_channel_recursive_reset_channel_init_recall(current,
						      recall_id_list);

      /* free allocated lists */
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	g_list_free(next_recall_id_list);
      }
    }
  }
  void ags_channel_recursive_init_recall_down(AgsChannel *current,
					      GList *recall_id_list)
  {
    AgsAudio *audio;
    GList *next_recall_id_list;

    if(current == NULL)
      return;

    audio = AGS_AUDIO(current->audio);

    /* retrieve next recall id list */
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
      next_recall_id_list = recall_id_list;
    }else{
      next_recall_id_list = NULL;

      while(recall_id_list != NULL){
	next_recall_id_list = g_list_concat(next_recall_id_list,
					    ags_recycling_container_get_child_recall_id(AGS_RECALL_ID(recall_id_list)->recycling_container));

	recall_id_list = recall_id_list->next;
      }
    }
    
    /* follow the links */
    ags_channel_recursive_resolve_recall_down_input(current,
						    next_recall_id_list);

    /* init audio */
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
      ags_channel_recursive_reset_audio_init_recall(audio,
						    next_recall_id_list);
    }

    ags_channel_recursive_reset_audio_init_recall(audio,
						  next_recall_id_list);

    /* init output */
    ags_channel_recursive_reset_channel_init_recall(current,
						    recall_id_list);

    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) == 0){
      g_list_free(next_recall_id_list);
    }
  }
  void ags_channel_tillrecycling_init_recall_up(AgsChannel *channel,
						GList *recall_id_list)
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
						      recall_id_list);
      

      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	break;
      }

      /* init audio */
      ags_channel_recursive_reset_audio_init_recall(audio,
						    recall_id_list);


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
						      recall_id_list);


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

  /* collect AgsDevoutPlays as lists */
  /* go down */
  channel_devout_play_list = NULL;
  channel_devout_play_list = ags_channel_tillrecycling_collect_devout_play_down(channel,
										channel_devout_play_list, FALSE);

  /* go up */
  link_devout_play_list = NULL;
  link_devout_play_list = ags_channel_recursive_collect_devout_play_up(link,
								       FALSE);

  /* retrieve lower */
  channel_recall_id_list = NULL;
  channel_recall_id_list = ags_channel_tillrecycling_collect_devout_play_down(channel,
									      channel_recall_id_list, TRUE);
  
  /* retrieve upper */
  link_recall_id_list = NULL;
  link_recall_id_list = ags_channel_recursive_collect_devout_play_up(link,
								     TRUE);

  /* retrieve invalid lower */
  channel_invalid_recall_id_list = NULL;
  channel_invalid_recall_id_list = ags_channel_tillrecycling_collect_devout_play_down(old_link_link,
										      channel_invalid_recall_id_list,
										      TRUE);

  /* retrieve invalid upper */
  link_invalid_recall_id_list = NULL;
  link_invalid_recall_id_list = ags_channel_recursive_collect_devout_play_up(old_channel_link,
									     TRUE);

  /* reset recall ids */
  /* go down */
  ags_channel_recursive_reset_recall_id_down(channel,
					    link_recall_id_list, link_devout_play_list,
					    channel_invalid_recall_id_list);

  /* go up */
  ags_channel_tillrecycling_reset_recall_id_up(link,
					       channel_recall_id_list, channel_devout_play_list,
					       link_invalid_recall_id_list);

  /* update AgsRecyclingContainer */
  list = link_recall_id_list;

  while(list != NULL){
    if(old_link_link != NULL){
      ags_recycling_container_reset_recycling(AGS_RECALL_ID(list->data)->recycling_container,
					      old_link_link->first_recycling, old_link_link->last_recycling,
					      channel->first_recycling, channel->last_recycling);
    }else{
      ags_recycling_container_reset_recycling(AGS_RECALL_ID(list->data)->recycling_container,
					      NULL, NULL,
					      channel->first_recycling, channel->last_recycling);
    }

    if(channel->first_recycling->parent != NULL){
      GList *parent;

      parent = AGS_CHANNEL(link->first_recycling->parent->channel)->recall_id;

      while(parent != NULL){
	ags_recycling_container_add_child(AGS_RECALL_ID(parent->data)->recycling_container,
					  AGS_RECALL_ID(list->data)->recycling_container);

	parent = parent->next;
      }
    }

    list = list->next;
  }

  /* unset recall ids */  
  /* go down */
  ags_channel_recursive_unset_recall_id_down(old_link_link,
					     link_invalid_recall_id_list);

  /* go up */
  ags_channel_tillrecycling_unset_recall_id_up(old_channel_link,
					       channel_invalid_recall_id_list);
  
  /* duplicate recalls */
  /* go down */
  ags_channel_recursive_duplicate_recall_down(channel,
					      link_recall_id_list);

  /* go up */
  ags_channel_tillrecycling_duplicate_recall_up(link,
						channel_recall_id_list);

  /* resolve recalls */
  /* go down */
  ags_channel_recursive_resolve_recall_down(channel,
					    link_recall_id_list);

  /* go up */
  ags_channel_tillrecycling_resolve_recall_up(link,
					      channel_recall_id_list);

  /* init recalls */
  for(stage = 0; stage < 3; stage++){
    /* go down */
    ags_channel_recursive_init_recall_down(channel,
					   link_recall_id_list);

    /* go up */
    ags_channel_tillrecycling_init_recall_up(link,
					     channel_recall_id_list);

  }

  /* free the lists */
  g_list_free(channel_devout_play_list);
  g_list_free(link_devout_play_list);

  g_list_free(channel_invalid_recall_id_list);
  g_list_free(link_invalid_recall_id_list);

  g_list_free(channel_recall_id_list);
  g_list_free(link_recall_id_list);
}

void
ags_channel_recall_id_set(AgsChannel *output, AgsRecallID *default_recall_id, gboolean ommit_own_channel,
			  guint mode, ...)
{
  AgsChannel *current;
  AgsRecallID *recall_id;
  char *key;
  guint stage;
  gboolean async_recall;
  va_list va_list;

  va_start(va_list, mode);

  switch(mode){
  case AGS_CHANNEL_RECALL_ID_RUN_STAGE:
    key = va_arg(va_list, char*);
    stage = va_arg(va_list, guint);
    break;
  }

  va_end(va_list);

  if((AGS_AUDIO_ASYNC & (AGS_AUDIO(output->audio)->flags)) != 0)
    async_recall = TRUE;
  else
    async_recall = FALSE;
  switch(async_recall){
  case TRUE:
    current = output->next_pad;
    while(current != NULL){
      recall_id = ags_recall_id_find_parent_recycling_container(current->recall_id,
								default_recall_id->recycling_container->parent);

      if(recall_id == NULL)
	return;

      switch(mode){
      case AGS_CHANNEL_RECALL_ID_RUN_STAGE:
	ags_recall_id_set_run_stage(default_recall_id, stage);
	break;
      case AGS_CHANNEL_RECALL_ID_CANCEL:
	recall_id->flags |= AGS_RECALL_ID_CANCEL;
	break;
      }

      current = current->next_pad;
    }

    current = output->prev_pad;

    while(current != NULL){
      recall_id = ags_recall_id_find_parent_recycling_container(current->recall_id,
								default_recall_id->recycling_container->parent);

      switch(mode){
      case AGS_CHANNEL_RECALL_ID_RUN_STAGE:
	ags_recall_id_set_run_stage(default_recall_id, stage);
	break;
      case AGS_CHANNEL_RECALL_ID_CANCEL:
	recall_id->flags |= AGS_RECALL_ID_CANCEL;
	break;
      }

      current = current->prev_pad;
    }
  case FALSE:
    if(ommit_own_channel)
      break;

    recall_id = ags_recall_id_find_parent_recycling_container(output->recall_id,
							      default_recall_id->recycling_container->parent);

    if(recall_id == NULL)
      return;

    switch(mode){
    case AGS_CHANNEL_RECALL_ID_RUN_STAGE:
      ags_recall_id_set_run_stage(default_recall_id, stage);

      break;
    case AGS_CHANNEL_RECALL_ID_CANCEL:
      recall_id->flags |= AGS_RECALL_ID_CANCEL;

      break;
    }
  }
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
