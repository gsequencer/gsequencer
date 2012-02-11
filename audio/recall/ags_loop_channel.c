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

void ags_loop_channel_resolve_dependencies(AgsRecall *recall);
AgsRecall* ags_loop_channel_duplicate(AgsRecall *recall,
				      AgsRecallID *recall_id,
				      guint n_params, GParameter *parameter);

void ags_loop_channel_loop_callback(AgsCountBeatsAudioRun *count_beats_audio_run,
				    guint nth_run,
				    AgsLoopChannel *loop_channel);

enum{
  PROP_0,
  PROP_CHANNEL,
  PROP_COUNT_BEATS_AUDIO_RUN,
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
  param_spec = g_param_spec_object("count_beats_audio_run\0",
				   "assigned AgsCountBeatsAudioRun\0",
				   "The pointer to a counter object which indicates when looping should happen\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT_BEATS_AUDIO_RUN,
				  param_spec);

  param_spec = g_param_spec_object("channel\0",
				   "assigned channel\0",
				   "The channel where looping should be applied\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) loop_channel;

  recall->duplicate = ags_loop_channel_duplicate;
  recall->resolve_dependencies = ags_loop_channel_resolve_dependencies;
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
  loop_channel->count_beats_audio_run = NULL;

  loop_channel->channel = NULL;

  loop_channel->template = NULL;
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
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      AgsCountBeatsAudioRun *count_beats_audio_run;
      gboolean is_template;

      count_beats_audio_run = (AgsCountBeatsAudioRun *) g_value_get_object(value);

      if(loop_channel->count_beats_audio_run == count_beats_audio_run)
	return;

      if(count_beats_audio_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(count_beats_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }


      if(loop_channel->count_beats_audio_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(loop_channel),
				       (AgsRecall *) loop_channel->count_beats_audio_run);
	}else{
	  if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(loop_channel)->flags)) != 0)
	    g_signal_handler_disconnect(G_OBJECT(loop_channel), loop_channel->loop_handler);
	}

	g_object_unref(loop_channel->count_beats_audio_run);
      }

      if(count_beats_audio_run != NULL){
	g_object_ref(count_beats_audio_run);

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(loop_channel),
				    ags_recall_dependency_new((GObject *) count_beats_audio_run));
	}else{
	  if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(loop_channel)->flags)) != 0)
	    loop_channel->loop_handler = g_signal_connect(G_OBJECT(loop_channel->count_beats_audio_run), "loop\0",
							  G_CALLBACK(ags_loop_channel_loop_callback), loop_channel);
	}
      }

      loop_channel->count_beats_audio_run = count_beats_audio_run;
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
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      g_value_set_object(value, loop_channel->count_beats_audio_run);
    }
    break;
  case PROP_CHANNEL:
    {
      g_value_set_object(value, loop_channel->channel);
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

  if(loop_channel->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(loop_channel->count_beats_audio_run));
  }

  if(loop_channel->channel != NULL){
    g_object_unref(G_OBJECT(loop_channel->channel));
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

  if(loop_channel->count_beats_audio_run != NULL)
    loop_channel->loop_handler = g_signal_connect(G_OBJECT(loop_channel->count_beats_audio_run), "loop\0",
						  G_CALLBACK(ags_loop_channel_loop_callback), loop_channel);
}

void
ags_loop_channel_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsLoopChannel *loop_channel;

  ags_loop_channel_parent_run_connectable_interface->disconnect(run_connectable);

  loop_channel = AGS_LOOP_CHANNEL(run_connectable);

  if(loop_channel->count_beats_audio_run != NULL)
    g_signal_handler_disconnect(G_OBJECT(loop_channel), loop_channel->loop_handler);
}

void
ags_loop_channel_resolve_dependencies(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsRecall *template;
  AgsLoopChannel *loop_channel;
  AgsRecallDependency *recall_dependency;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  GList *list;
  AgsGroupId group_id;
  guint i, i_stop;

  loop_channel = AGS_LOOP_CHANNEL(recall);

  template = loop_channel->template;

  list = template->dependencies;

  channel = loop_channel->channel;
  audio = AGS_AUDIO(channel->audio);

  group_id = recall->recall_id->child_group_id;

  count_beats_audio_run = NULL;
  i_stop = 1;

  for(i = 0; i < i_stop && list != NULL;){
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    if(AGS_IS_COUNT_BEATS_AUDIO_RUN(recall_dependency->dependency)){
      count_beats_audio_run = (AgsCountBeatsAudioRun *) ags_recall_dependency_resolve(recall_dependency, group_id);

      i++;
    }

    list = list->next;
  }

  g_object_set(G_OBJECT(recall),
	       "count_beats_audio_run\0", count_beats_audio_run,
	       NULL);
}

AgsRecall*
ags_loop_channel_duplicate(AgsRecall *recall,
			   AgsRecallID *recall_id,
			   guint n_params, GParameter *parameter)
{
  AgsLoopChannel *loop_channel, *copy;

  loop_channel = AGS_LOOP_CHANNEL(recall);
  copy = (AgsLoopChannel *) AGS_RECALL_CLASS(ags_loop_channel_parent_class)->duplicate(recall,
										       recall_id,
										       n_params, parameter);

  //TODO:JK this is really ugly
  copy->template = recall;
  copy->channel = loop_channel->channel;

  return((AgsRecall *) copy);
}

void 
ags_loop_channel_loop_callback(AgsCountBeatsAudioRun *count_beats_audio_run,
			       guint nth_run,
			       AgsLoopChannel *loop_channel)
{
  AgsDevout *devout;
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;

  printf("%u: %u\n\0", AGS_RECALL_CHANNEL_RUN(loop_channel)->run_order, nth_run);

  if(AGS_RECALL_CHANNEL_RUN(loop_channel)->run_order != nth_run)
    return;

  devout = AGS_DEVOUT(AGS_AUDIO(loop_channel->channel->audio)->devout);

  recycling = loop_channel->channel->first_recycling;
 
  while(recycling != loop_channel->channel->last_recycling->next){
    audio_signal = ags_audio_signal_new((GObject *) devout,
					(GObject *) recycling,
					(GObject *) AGS_RECALL(loop_channel)->recall_id);
    ags_recycling_create_audio_signal_with_defaults(recycling, audio_signal);
    audio_signal->stream_current = audio_signal->stream_beginning;

    ags_connectable_connect(AGS_CONNECTABLE(audio_signal));    
    ags_recycling_add_audio_signal(recycling,
				   audio_signal);
    
    
    recycling = recycling->next;
  }
}

AgsLoopChannel*
ags_loop_channel_new(AgsChannel *channel,
		     AgsCountBeatsAudioRun *count_beats_audio_run,
		     gboolean is_template)
{
  AgsLoopChannel *loop_channel;

  loop_channel = (AgsLoopChannel *) g_object_new(AGS_TYPE_LOOP_CHANNEL,
						 "channel\0", channel,
						 "count_beats_audio_run\0", count_beats_audio_run,
						 NULL);

  if(is_template){
    GList *list;

    AGS_RECALL(loop_channel)->flags = AGS_RECALL_TEMPLATE;

    list = NULL;
    //    list = g_list_prepend(list, ags_recall_dependency_new(G_OBJECT(count_beats_audio_run)));
  }

  return(loop_channel);
}
