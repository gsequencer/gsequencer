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

#include <ags/audio/ags_recall_channel_run.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_packable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_copy_pattern_channel_run.h>

void ags_recall_channel_run_class_init(AgsRecallChannelRunClass *recall_channel_run);
void ags_recall_channel_runconnectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_channel_run_packable_interface_init(AgsPackableInterface *packable);
void ags_recall_channel_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_recall_channel_run_init(AgsRecallChannelRun *recall_channel_run);
void ags_recall_channel_run_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_recall_channel_run_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_recall_channel_run_connect(AgsConnectable *connectable);
gboolean ags_recall_channel_run_pack(AgsPackable *packable, GObject *container);
gboolean ags_recall_channel_run_unpack(AgsPackable *packable);
void ags_recall_channel_run_disconnect(AgsConnectable *connectable);
void ags_recall_channel_run_run_connect(AgsRunConnectable *run_connectable);
void ags_recall_channel_run_run_disconnect(AgsRunConnectable *run_connectable);
void ags_recall_channel_run_finalize(GObject *gobject);

AgsRecall* ags_recall_channel_run_duplicate(AgsRecall *recall,
					    AgsRecallID *recall_id,
					    guint n_params, GParameter *parameter);

AgsGroupId ags_recall_channel_run_real_get_audio_run_group_id(AgsRecallChannelRun *recall_channel_run);
void ags_recall_channel_run_real_run_order_changed(AgsRecallChannelRun *recall_channel_run,
						   guint run_order);

enum{
  GET_AUDIO_RUN_GROUP_ID,
  RUN_ORDER_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_CHANNEL,
  PROP_RECALL_AUDIO_RUN,
  PROP_RECALL_CHANNEL,
};

static gpointer ags_recall_channel_run_parent_class = NULL;
static AgsConnectableInterface* ags_recall_channel_run_parent_connectable_interface;
static AgsPackableInterface* ags_recall_channel_run_parent_packable_interface;
static AgsRunConnectableInterface *ags_recall_channel_run_parent_run_connectable_interface;

static guint recall_channel_run_signals[LAST_SIGNAL];

GType
ags_recall_channel_run_get_type()
{
  static GType ags_type_recall_channel_run = 0;

  if(!ags_type_recall_channel_run){
    static const GTypeInfo ags_recall_channel_run_info = {
      sizeof (AgsRecallChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_channel_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_runconnectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_packable_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_run_packable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_run_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_channel_run = g_type_register_static(AGS_TYPE_RECALL,
							 "AgsRecallChannelRun\0",
							 &ags_recall_channel_run_info,
							 0);
    
    g_type_add_interface_static(ags_type_recall_channel_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_channel_run,
				AGS_TYPE_PACKABLE,
				&ags_packable_interface_info);
    
    g_type_add_interface_static(ags_type_recall_channel_run,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_recall_channel_run);
}

void
ags_recall_channel_run_class_init(AgsRecallChannelRunClass *recall_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_channel_run_parent_class = g_type_class_peek_parent(recall_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_channel_run;

  gobject->set_property = ags_recall_channel_run_set_property;
  gobject->get_property = ags_recall_channel_run_get_property;

  gobject->finalize = ags_recall_channel_run_finalize;

  /* properties */
  param_spec = g_param_spec_object("channel\0",
				   "AgsChannel of this recall\0",
				   "The AgsChannel of this recall\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  param_spec = g_param_spec_object("recall_audio_run\0",
				   "AgsRecallAudioRun of this recall\0",
				   "The AgsRecallAudioRun which this recall needs\0",
				   AGS_TYPE_RECALL_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_AUDIO_RUN,
				  param_spec);

  param_spec = g_param_spec_object("recall_channel\0",
				   "AsgRecallChannel of this recall\0",
				   "The AgsRecallChannel which this recall needs\0",
				   AGS_TYPE_RECALL_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CHANNEL,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_channel_run;

  recall->duplicate = ags_recall_channel_run_duplicate;

  /* AgsRecallChannelRunClass */
  recall_channel_run->get_audio_run_group_id = ags_recall_channel_run_real_get_audio_run_group_id;
  recall_channel_run->run_order_changed = ags_recall_channel_run_real_run_order_changed;

  /* signals */
  recall_channel_run_signals[RUN_ORDER_CHANGED] =
    g_signal_new("run_order_changed\0",
		 G_TYPE_FROM_CLASS (recall_channel_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallChannelRunClass, run_order_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /* AgsRecallChannelRunClass */
  recall_channel_run->get_audio_run_group_id = NULL;

  recall_channel_run_signals[GET_AUDIO_RUN_GROUP_ID] =
    g_signal_new("get_audio_run_group_id\0",
		 G_TYPE_FROM_CLASS (recall_channel_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallChannelRunClass, get_audio_run_group_id),
		 NULL, NULL,
		 g_cclosure_user_marshal_ULONG__VOID,
		 G_TYPE_ULONG, 0);
}

void
ags_recall_channel_runconnectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_recall_channel_run_connectable_parent_interface;

  ags_recall_channel_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_channel_run_connect;
  connectable->disconnect = ags_recall_channel_run_disconnect;
}

void
ags_recall_channel_run_packable_interface_init(AgsPackableInterface *packable)
{
  ags_recall_channel_run_parent_packable_interface = g_type_interface_peek_parent(packable);

  packable->pack = ags_recall_channel_run_pack;
  packable->unpack = ags_recall_channel_run_unpack;
}

void
ags_recall_channel_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_recall_channel_run_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_recall_channel_run_run_connect;
  run_connectable->disconnect = ags_recall_channel_run_run_disconnect;
}

void
ags_recall_channel_run_init(AgsRecallChannelRun *recall_channel_run)
{
  recall_channel_run->channel = NULL;

  recall_channel_run->recall_audio_run = NULL;
  recall_channel_run->recall_channel = NULL;

  recall_channel_run->run_order = 0;
}


void
ags_recall_channel_run_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsRecallChannelRun *recall_channel_run;

  recall_channel_run = AGS_RECALL_CHANNEL_RUN(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(recall_channel_run->channel == channel)
	return;

      if(recall_channel_run->channel != NULL){
	g_object_unref(G_OBJECT(recall_channel_run->channel));
      }

      if(channel != NULL){
	g_object_ref(G_OBJECT(channel));
      }

      recall_channel_run->channel = channel;
    }
    break;
  case PROP_RECALL_AUDIO_RUN:
    {
      AgsRecallAudioRun *recall_audio_run;

      recall_audio_run = (AgsRecallAudioRun *) g_value_get_object(value);

      if(recall_channel_run->recall_audio_run == recall_audio_run)
	return;

      if(recall_channel_run->recall_audio_run != NULL){
	g_object_unref(G_OBJECT(recall_channel_run->recall_audio_run));
      }

      if(recall_audio_run != NULL){
	g_object_ref(G_OBJECT(recall_audio_run));
      }

      recall_channel_run->recall_audio_run = recall_audio_run;
    }
    break;
  case PROP_RECALL_CHANNEL:
    {
      AgsRecallChannel *recall_channel;

      recall_channel = (AgsRecallChannel *) g_value_get_object(value);

      if(recall_channel_run->recall_channel == recall_channel)
	return;

      if(recall_channel_run->recall_channel != NULL){
	g_object_unref(G_OBJECT(recall_channel_run->recall_channel));
      }

      if(recall_channel != NULL){
	g_object_ref(G_OBJECT(recall_channel));
      }

      recall_channel_run->recall_channel = recall_channel;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_channel_run_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsRecallChannelRun *recall_channel_run;

  recall_channel_run = AGS_RECALL_CHANNEL_RUN(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value, recall_channel_run->channel);
    }
    break;
  case PROP_RECALL_AUDIO_RUN:
    {
      g_value_set_object(value, recall_channel_run->recall_audio_run);
    }
    break;
  case PROP_RECALL_CHANNEL:
    {
      g_value_set_object(value, recall_channel_run->recall_channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_channel_run_finalize(GObject *gobject)
{
  AgsRecallChannelRun *recall_channel_run;

  recall_channel_run = AGS_RECALL_CHANNEL_RUN(gobject);

  if(recall_channel_run->recall_audio_run != NULL)
    g_object_unref(G_OBJECT(recall_channel_run->recall_audio_run));
  
  if(recall_channel_run->recall_channel != NULL)
    g_object_unref(G_OBJECT(recall_channel_run->recall_channel));

  G_OBJECT_CLASS(ags_recall_channel_run_parent_class)->finalize(gobject);
}

void
ags_recall_channel_run_connect(AgsConnectable *connectable)
{
  ags_recall_channel_run_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_channel_run_disconnect(AgsConnectable *connectable)
{
  ags_recall_channel_run_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

gboolean
ags_recall_channel_run_pack(AgsPackable *packable, GObject *container)
{
  AgsRecallContainer *recall_container;
  GList *list;
  AgsGroupId group_id;

  if(ags_recall_channel_run_parent_packable_interface->pack(packable, container))
    return(TRUE);

  recall_container = AGS_RECALL_CONTAINER(container);

  recall_container->recall_channel_run = g_list_prepend(recall_container->recall_channel_run,
							AGS_RECALL(packable));

  /* set AgsRecallAudioRun */
  list = recall_container->recall_audio_run;

  if(AGS_RECALL(packable)->recall_id != NULL){
    group_id = (AGS_RECALL_INPUT_ORIENTATED & (AGS_RECALL(packable)->flags) != 0) ?
      AGS_RECALL(packable)->recall_id->group_id:
      AGS_RECALL(packable)->recall_id->child_group_id;
      
    list = ags_recall_find_group_id(list,
				    group_id);

    if(list != NULL){
      g_object_set(G_OBJECT(packable),
		   "recall_audio_run\0", AGS_RECALL_AUDIO_RUN(list->data),
		   NULL);
    }
  }else if((AGS_RECALL_TEMPLATE & (AGS_RECALL(packable)->flags)) != 0){
    list = ags_recall_find_template(list);

    if(list != NULL){
      g_object_set(G_OBJECT(packable),
		   "recall_audio_run\0", AGS_RECALL_AUDIO_RUN(list->data),
		   NULL);
    }
  }

  /* set AgsRecallChannel */
  if(AGS_RECALL_CHANNEL_RUN(packable)->channel != NULL){
    list = recall_container->recall_channel;

    if((list = ags_recall_find_provider(list,
					G_OBJECT(AGS_RECALL_CHANNEL_RUN(packable)->channel))) != NULL){
      g_object_set(G_OBJECT(packable),
		   "recall_channel\0", AGS_RECALL_CHANNEL(list->data),
		   NULL);
    }
  }

  return(FALSE);
}

gboolean
ags_recall_channel_run_unpack(AgsPackable *packable)
{
  AgsRecall *recall;
  AgsRecallContainer *recall_container;
  GList *list;
  AgsGroupId group_id;

  recall = AGS_RECALL(packable);

  if(recall == NULL)
    return(TRUE);

  recall_container = AGS_RECALL_CONTAINER(recall->container);

  if(recall_container == NULL)
    return(TRUE);

  /* ref */
  g_object_ref(recall);
  g_object_ref(recall_container);

  /* unset AgsRecallAudioRun */
  g_object_set(G_OBJECT(packable),
	       "recall_audio_run\0", NULL,
	       NULL);

  /* unset AgsRecallChannel */
  g_object_set(G_OBJECT(packable),
	       "recall_channel\0", NULL,
	       NULL);

  /* call parent */
  if(ags_recall_channel_run_parent_packable_interface->unpack(packable)){
    g_object_unref(recall);
    g_object_unref(recall_container);

    return(TRUE);
  }

  /* remove from list */
  recall_container->recall_channel_run = g_list_remove(recall_container->recall_channel_run,
						       recall);

  /* unref */
  g_object_unref(recall);
  g_object_unref(recall_container);

  return(FALSE);
}

void
ags_recall_channel_run_run_connect(AgsConnectable *connectable)
{
  ags_recall_channel_run_parent_run_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_channel_run_run_disconnect(AgsConnectable *connectable)
{
  ags_recall_channel_run_parent_run_connectable_interface->disconnect(connectable);

  /* empty */
}

AgsRecall*
ags_recall_channel_run_duplicate(AgsRecall *recall,
				 AgsRecallID *recall_id,
				 guint n_params, GParameter *parameter)
{
  AgsRecallChannelRun *recall_channel_run, *copy;
  AgsRecallContainer *container;
  AgsRecallAudioRun *recall_audio_run;
  GList *recall_audio_run_list;
  guint new_n_params;
  GParameter *new_parameter;

  recall_channel_run = AGS_RECALL_CHANNEL_RUN(recall);

  ags_parameter_grow(AGS_TYPE_RECALL_CHANNEL_RUN,
		     n_params, parameter,
		     &new_n_params, &new_parameter,
		     "channel\0", recall_channel_run->channel,
		     NULL);
  copy = AGS_RECALL_CHANNEL_RUN(AGS_RECALL_CLASS(ags_recall_channel_run_parent_class)->duplicate(recall,
												 recall_id,
												 new_n_params, new_parameter));
  
  /* moved to AgsPackable * /
  container = AGS_RECALL_CONTAINER(recall->container);

  if(container != NULL){
    /* set recall audio run * /
    recall_audio_run_list = container->recall_audio_run;
    recall_audio_run_list = ags_recall_find_group_id(recall_audio_run_list,
						     recall_id->parent_group_id);
    
    if(recall_audio_run_list != NULL){
      recall_audio_run = AGS_RECALL_AUDIO_RUN(recall_audio_run_list->data);
      
      g_object_set(G_OBJECT(copy),
		   "recall_audio_run\0", G_OBJECT(recall_audio_run),
		   NULL);
    }
  }

  /* set recall channel * /
  g_object_set(G_OBJECT(copy),
	       "recall_channel\0", G_OBJECT(recall_channel_run->recall_channel),
	       NULL);
  */

  return((AgsRecall *) copy);
}

void
ags_recall_channel_run_real_run_order_changed(AgsRecallChannelRun *recall_channel_run,
					      guint run_order)
{
  recall_channel_run->run_order = run_order;
}

void
ags_recall_channel_run_run_order_changed(AgsRecallChannelRun *recall_channel_run,
					 guint run_order)
{
  g_return_if_fail(AGS_IS_RECALL(recall_channel_run));

  g_object_ref(G_OBJECT(recall_channel_run));
  g_signal_emit(G_OBJECT(recall_channel_run),
		recall_channel_run_signals[RUN_ORDER_CHANGED], 0,
		run_order);
  g_object_unref(G_OBJECT(recall_channel_run));
}

AgsGroupId
ags_recall_channel_run_real_get_audio_run_group_id(AgsRecallChannelRun *recall_channel_run)
{
  AgsAudio *audio;
  AgsRecall *recall;
  AgsGroupId group_id;

  recall = AGS_RECALL(recall_channel_run);

  if(recall->recall_id == NULL)
    return(G_MAXULONG);

  audio = AGS_AUDIO(recall_channel_run->channel->audio);

  if((AGS_RECALL_INPUT_ORIENTATED & (recall->flags)) != 0){
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
      group_id = recall->recall_id->parent_group_id;
    }else{
      group_id = recall->recall_id->group_id;
    }
  }else if((AGS_RECALL_OUTPUT_ORIENTATED & (recall->flags)) != 0){
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      group_id = recall->recall_id->child_group_id;
    }else{
      group_id = recall->recall_id->group_id;
    }
  }else{
    group_id = recall->recall_id->group_id;
  }

  return(group_id);
}

AgsGroupId
ags_recall_channel_run_get_audio_run_group_id(AgsRecallChannelRun *recall_channel_run)
{
  AgsGroupId group_id;

  g_return_val_if_fail(AGS_IS_RECALL_CHANNEL_RUN(recall_channel_run), G_MAXULONG);

  g_object_ref(G_OBJECT(recall_channel_run));
  g_signal_emit(G_OBJECT(recall_channel_run),
		recall_channel_run_signals[GET_AUDIO_RUN_GROUP_ID], 0,
		&group_id);
  g_object_unref(G_OBJECT(recall_channel_run));

  return(group_id);
}

guint
ags_recall_channel_run_get_run_order(AgsRecallChannelRun *recall_channel_run)
{
  return(recall_channel_run->run_order);
}

AgsRecallChannelRun*
ags_recall_channel_run_new()
{
  AgsRecallChannelRun *recall_channel_run;

  recall_channel_run = (AgsRecallChannelRun *) g_object_new(AGS_TYPE_RECALL_CHANNEL_RUN, NULL);

  return(recall_channel_run);
}
