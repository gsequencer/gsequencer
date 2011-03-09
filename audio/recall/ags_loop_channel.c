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

#include <ags/audio/recall/ags_loop_channel.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>
#include <ags/object/ags_countable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_id.h>

void ags_loop_channel_class_init(AgsLoopChannelClass *loop_channel);
void ags_loop_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_loop_channel_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_loop_channel_init(AgsLoopChannel *loop_channel);
void ags_loop_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_loop_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_loop_channel_finalize(GObject *gobject);
void ags_loop_channel_connect(AgsConnectable *connectable);
void ags_loop_channel_disconnect(AgsConnectable *connectable);
void ags_loop_channel_run_connect(AgsRunConnectable *run_connectable);
void ags_loop_channel_run_disconnect(AgsRunConnectable *run_connectable);

AgsRecall* ags_loop_channel_duplicate(AgsRecall *recall,
				      AgsRecallID *recall_id);

void ags_loop_channel_tic_alloc_callback(AgsDelayAudioRun *delay_audio_run,
					 guint nth_run,
					 AgsLoopChannel *loop_channel);

enum{
  PROP_0,
  PROP_CHANNEL,
  PROP_DELAY_AUDIO_RUN,
  PROP_COUNTER,
};

static gpointer ags_loop_channel_parent_class = NULL;
static AgsConnectableInterface *ags_loop_channel_parent_connectable_interface;
static AgsRunConnectableInterface *ags_loop_channel_parent_run_connectable_interface;

GType
ags_loop_channel_get_type()
{
  static GType ags_type_loop_channel = 0;

  if(!ags_type_loop_channel){
    static const GTypeInfo ags_loop_channel_info = {
      sizeof (AgsLoopChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_loop_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLoopChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_loop_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_loop_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_loop_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_loop_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
						   "AgsLoopChannel\0",
						   &ags_loop_channel_info,
						   0);
    g_type_add_interface_static(ags_type_loop_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_loop_channel,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return (ags_type_loop_channel);
}

void
ags_loop_channel_class_init(AgsLoopChannelClass *loop_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_loop_channel_parent_class = g_type_class_peek_parent(loop_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) loop_channel;

  gobject->set_property = ags_loop_channel_set_property;
  gobject->get_property = ags_loop_channel_get_property;

  gobject->finalize = ags_loop_channel_finalize;

  /* properties */
  param_spec = g_param_spec_object("channel\0",
				   "assigned channel\0",
				   "The channel where looping should be applied\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  param_spec = g_param_spec_object("delay_audio_run\0",
				   "assigned AgsDelayAudioRun\0",
				   "The AgsDelayAudioRun which emits tic_alloc signal\0",
				   AGS_TYPE_DELAY_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_AUDIO_RUN,
				  param_spec);

  param_spec = g_param_spec_uint("nth_run\0",
				 "nth run in the queue\0",
				 "The nth run in the queue of other channels\0",
				 0,
				 65536,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNTER,
				  param_spec);

  param_spec = g_param_spec_object("counter\0",
				   "pointer to a counter\0",
				   "The pointer to a counter object which indicates when looping should happen\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNTER,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) loop_channel;

  recall->duplicate = ags_loop_channel_duplicate;
}

void
ags_loop_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_loop_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_loop_channel_connect;
  connectable->disconnect = ags_loop_channel_disconnect;
}

void
ags_loop_channel_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_loop_channel_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_loop_channel_run_connect;
  run_connectable->disconnect = ags_loop_channel_run_disconnect;
}

void
ags_loop_channel_init(AgsLoopChannel *loop_channel)
{
  loop_channel->delay_audio_run = NULL;

  loop_channel->channel = NULL;

  loop_channel->counter = NULL;
}

void
ags_loop_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsLoopChannel *loop_channel;

  loop_channel = AGS_LOOP_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(loop_channel->channel == channel)
	return;

      if(loop_channel->channel != NULL)
	g_object_unref(loop_channel->channel);

      if(channel != NULL)
	g_object_ref(channel);

      loop_channel->channel = channel;
    }
    break;
  case PROP_DELAY_AUDIO_RUN:
    {
      AgsDelayAudioRun *delay_audio_run;

      delay_audio_run = (AgsDelayAudioRun *) g_value_get_object(value);

      if(loop_channel->delay_audio_run == delay_audio_run)
	return;

      if(loop_channel->delay_audio_run != NULL){
	if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(loop_channel)->flags)) != 0)
	  g_signal_handler_disconnect(G_OBJECT(loop_channel), loop_channel->tic_alloc_handler);

	g_object_unref(loop_channel->delay_audio_run);
      }

      if(delay_audio_run != NULL){
	if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(loop_channel)->flags)) != 0)
	  loop_channel->tic_alloc_handler = g_signal_connect(G_OBJECT(loop_channel->delay_audio_run), "tic_alloc_output\0",
							     G_CALLBACK(ags_loop_channel_tic_alloc_callback), loop_channel);

	g_object_ref(delay_audio_run);
      }

      loop_channel->delay_audio_run = delay_audio_run;
    }
    break;
  case PROP_COUNTER:
    {
      GObject *counter;

      counter = (GObject *) g_value_get_object(value);

      if(loop_channel->counter == counter)
	return;

      if(loop_channel->counter != NULL)
	g_object_unref(loop_channel->counter);

      if(counter != NULL)
	g_object_ref(counter);

      loop_channel->counter = counter;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_loop_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsLoopChannel *loop_channel;

  loop_channel = AGS_LOOP_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value, loop_channel->channel);
    }
    break;
  case PROP_DELAY_AUDIO_RUN:
    {
      g_value_set_object(value, loop_channel->delay_audio_run);
    }
    break;
  case PROP_COUNTER:
    {
      g_value_set_object(value, loop_channel->counter);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_loop_channel_finalize(GObject *gobject)
{
  AgsLoopChannel *loop_channel;

  loop_channel = AGS_LOOP_CHANNEL(gobject);

  if(loop_channel->delay_audio_run != NULL){
    g_object_unref(G_OBJECT(loop_channel->delay_audio_run));
  }

  if(loop_channel->channel != NULL){
    g_object_unref(G_OBJECT(loop_channel->channel));
  }

  if(loop_channel->counter != NULL){
    g_object_unref(G_OBJECT(loop_channel->counter));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_loop_channel_parent_class)->finalize(gobject);
}

void
ags_loop_channel_connect(AgsConnectable *connectable)
{
  ags_loop_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_loop_channel_disconnect(AgsConnectable *connectable)
{
  ags_loop_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_loop_channel_run_connect(AgsRunConnectable *run_connectable)
{
  AgsLoopChannel *loop_channel;

  ags_loop_channel_parent_run_connectable_interface->connect(run_connectable);

  loop_channel = AGS_LOOP_CHANNEL(run_connectable);

  if(loop_channel->delay_audio_run != NULL)
    loop_channel->tic_alloc_handler = g_signal_connect(G_OBJECT(loop_channel->delay_audio_run), "tic_alloc_output\0",
						       G_CALLBACK(ags_loop_channel_tic_alloc_callback), loop_channel);
}

void
ags_loop_channel_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsLoopChannel *loop_channel;

  ags_loop_channel_parent_run_connectable_interface->disconnect(run_connectable);

  loop_channel = AGS_LOOP_CHANNEL(run_connectable);

  if(loop_channel->delay_audio_run != NULL)
    g_signal_handler_disconnect(G_OBJECT(loop_channel), loop_channel->tic_alloc_handler);

}

AgsRecall*
ags_loop_channel_duplicate(AgsRecall *recall,
			   AgsRecallID *recall_id)
{
  AgsLoopChannel *loop_channel, *copy;
  AgsRecallContainer *container;
  AgsDelayAudioRun *delay_audio_run;
  GObject *counter;
  GList *list;

  loop_channel = AGS_LOOP_CHANNEL(recall);
  copy = (AgsLoopChannel *) AGS_RECALL_CLASS(ags_loop_channel_parent_class)->duplicate(recall,
										       recall_id);

  container = AGS_RECALL_CONTAINER(AGS_RECALL(loop_channel->delay_audio_run)->container);
  list = ags_recall_find_group_id(container->recall_audio_run, recall_id->group_id);
  delay_audio_run = AGS_DELAY_AUDIO_RUN(list->data);

  container = AGS_RECALL_CONTAINER(AGS_RECALL(loop_channel->counter)->container);
  list = ags_recall_find_group_id(container->recall_audio_run, recall_id->group_id);
  counter = G_OBJECT(list->data);

  g_object_set(G_OBJECT(copy),
	       "channel\0", loop_channel->channel,
	       "delay_audio_run\0", delay_audio_run,
	       "counter\0", counter,
	       NULL);

  
  return((AgsRecall *) copy);
}

void 
ags_loop_channel_tic_alloc_callback(AgsDelayAudioRun *delay_audio_run,
				    guint nth_run,
				    AgsLoopChannel *loop_channel)
{
  AgsDevout *devout;
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;

  if(AGS_RECALL_CHANNEL_RUN(loop_channel)->run_order != nth_run ||
      AGS_COUNTABLE_GET_INTERFACE(loop_channel->counter)->get_counter(AGS_COUNTABLE(loop_channel->counter)) != 0)
    return;

  devout = AGS_DEVOUT(AGS_AUDIO(loop_channel->channel->audio)->devout);

  recycling = loop_channel->channel->first_recycling;
 
  while(recycling != loop_channel->channel->last_recycling->next){
    audio_signal = ags_audio_signal_new((GObject *) devout,
					(GObject *) recycling,
					(GObject *) AGS_RECALL(loop_channel)->recall_id);
    ags_connectable_connect(AGS_CONNECTABLE(audio_signal));
    
    ags_recycling_add_audio_signal(recycling,
				   audio_signal);
    audio_signal->stream_current = audio_signal->stream_beginning;
    
    
    recycling = recycling->next;
  }
}

AgsLoopChannel*
ags_loop_channel_new(AgsChannel *channel,
		     AgsDelayAudioRun *delay_audio_run,
		     GObject *counter)
{
  AgsLoopChannel *loop_channel;

  loop_channel = (AgsLoopChannel *) g_object_new(AGS_TYPE_LOOP_CHANNEL,
						 "channel\0", channel,
						 "delay_audio_run\0", delay_audio_run,
						 "counter\0", counter,
						 NULL);

  return(loop_channel);
}
