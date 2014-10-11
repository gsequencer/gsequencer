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

#include <ags/main.h>

#include <ags/lib/ags_parameter.h>

#include <ags/object/ags_marshal.h>
#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_packable.h>
#include <ags/object/ags_dynamic_connectable.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_recycling.h>

#include <ags/audio/recall/ags_copy_recycling.h>

#include <ags/audio/task/ags_cancel_recall.h>

void ags_recall_channel_run_class_init(AgsRecallChannelRunClass *recall_channel_run);
void ags_recall_channel_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_channel_run_packable_interface_init(AgsPackableInterface *packable);
void ags_recall_channel_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
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
void ags_recall_channel_run_disconnect(AgsConnectable *connectable);
gboolean ags_recall_channel_run_pack(AgsPackable *packable, GObject *container);
gboolean ags_recall_channel_run_unpack(AgsPackable *packable);
void ags_recall_channel_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_recall_channel_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_recall_channel_run_finalize(GObject *gobject);

AgsRecall* ags_recall_channel_run_duplicate(AgsRecall *recall,
					    AgsRecallID *recall_id,
					    guint *n_params, GParameter *parameter);

void ags_recall_channel_run_map_recall_recycling(AgsRecallChannelRun *recall_channel_run);

void ags_recall_channel_run_remap_child_source(AgsRecallChannelRun *recall_channel_run,
					       AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
					       AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region);
void ags_recall_channel_run_remap_child_destination(AgsRecallChannelRun *recall_channel_run,
						    AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
						    AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region);
void ags_recall_channel_run_source_recycling_changed_callback(AgsChannel *channel,
							      AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							      AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							      AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
							      AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
							      AgsRecallChannelRun *recall_channel_run);

void ags_recall_channel_run_destination_recycling_changed_callback(AgsChannel *channel,
								   AgsRecycling *old_start_region, AgsRecycling *old_end_region,
								   AgsRecycling *new_start_region, AgsRecycling *new_end_region,
								   AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
								   AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
								   AgsRecallChannelRun *recall_channel_run);

void ags_recall_channel_run_real_run_order_changed(AgsRecallChannelRun *recall_channel_run,
						   guint run_order);

/**
 * SECTION:ags_recall_channel_run
 * @short_description: dynamic channel context of recall
 * @title: AgsRecallChannelRun
 * @section_id:
 * @include: ags/audio/ags_recall_channel_run.h
 *
 * #AgsRecallChannelRun acts as channel recall run.
 */

enum{
  RUN_ORDER_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_AUDIO_CHANNEL,
  PROP_DESTINATION,
  PROP_SOURCE,
  PROP_RECALL_AUDIO_RUN,
  PROP_RECALL_CHANNEL,
};

static gpointer ags_recall_channel_run_parent_class = NULL;
static AgsConnectableInterface* ags_recall_channel_run_parent_connectable_interface;
static AgsPackableInterface* ags_recall_channel_run_parent_packable_interface;
static AgsDynamicConnectableInterface *ags_recall_channel_run_parent_dynamic_connectable_interface;

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
      (GInterfaceInitFunc) ags_recall_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_packable_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_run_packable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_run_dynamic_connectable_interface_init,
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
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
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
  /**
   * AgsRecallChannelRun:audio-channel:
   *
   * The audio channel to output.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_uint("audio-channel\0",
				 "assigned audio channel\0",
				 "The audio channel this recall is assigned to\0",
				 0, 65535,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);


  /**
   * AgsRecallChannelRun:recall-audio-run:
   *
   * The audio run opposite of this recall.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("recall-audio-run\0",
				   "AgsRecallAudioRun of this recall\0",
				   "The AgsRecallAudioRun which this recall needs\0",
				   AGS_TYPE_RECALL_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_AUDIO_RUN,
				  param_spec);

  /**
   * AgsRecallChannelRun:recall-channel:
   *
   * The channel context of this recall.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("recall-channel\0",
				   "AsgRecallChannel of this recall\0",
				   "The AgsRecallChannel which this recall needs\0",
				   AGS_TYPE_RECALL_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CHANNEL,
				  param_spec);

  /**
   * AgsRecallChannelRun:destination:
   *
   * The channel to do output to.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("destination\0",
				   "destination of output\0",
				   "The destination AgsChannel where it will output to\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DESTINATION,
				  param_spec);

  /**
   * AgsRecallChannelRun:source:
   *
   * The channel to do input from.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("source\0",
				   "source of input\0",
				   "The source AgsChannel where it will take the input from\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOURCE,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_channel_run;

  recall->duplicate = ags_recall_channel_run_duplicate;

  /* AgsRecallChannelRunClass */
  recall_channel_run->run_order_changed = ags_recall_channel_run_real_run_order_changed;

  /* signals */
  /**
   * AgsRecallChannelRun::run-order-changed:
   * @recall_channel_run: the object run order changed
   * @run_order: nth run of parent recycling. 
   *
   * The ::run-order-changed signal is invoked as changing link of destination
   * during playback.
   */
  recall_channel_run_signals[RUN_ORDER_CHANGED] =
    g_signal_new("run-order-changed\0",
		 G_TYPE_FROM_CLASS (recall_channel_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallChannelRunClass, run_order_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);
}

void
ags_recall_channel_run_connectable_interface_init(AgsConnectableInterface *connectable)
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
ags_recall_channel_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_recall_channel_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_recall_channel_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_recall_channel_run_disconnect_dynamic;
}

void
ags_recall_channel_run_init(AgsRecallChannelRun *recall_channel_run)
{
  recall_channel_run->audio_channel = 0;

  recall_channel_run->recall_audio_run = NULL;
  recall_channel_run->recall_channel = NULL;

  recall_channel_run->source = NULL;
  recall_channel_run->destination = NULL;

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
  case PROP_AUDIO_CHANNEL:
    {
      recall_channel_run->audio_channel = g_value_get_uint(value);
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
  case PROP_DESTINATION:
    {
      AgsChannel *destination;
      AgsChannel *old_destination;
      AgsRecycling *new_start_region, *new_end_region;
      AgsRecycling *old_start_region, *old_end_region;

      destination = (AgsChannel *) g_value_get_object(value);

      if(recall_channel_run->destination == destination)
	return;

      old_destination = recall_channel_run->destination;

      if(old_destination != NULL){
	old_start_region = old_destination->first_recycling;
	old_end_region = old_destination->last_recycling;
      }else{
	old_start_region = NULL;
	old_end_region = NULL;
      }

      if(destination != NULL){    
	g_object_ref(G_OBJECT(destination));

	new_start_region = destination->first_recycling;
	new_end_region = destination->last_recycling;
      }else{
	new_start_region = NULL;
	new_end_region = NULL;
      }

      recall_channel_run->destination = destination;

      /* child destination */
      if(destination == recall_channel_run->source)
	g_warning("destination == recall_channel_run->source\0");

      if(old_destination != NULL)
	g_object_unref(G_OBJECT(old_destination));
    }
    break;
  case PROP_SOURCE:
    {
      AgsChannel *source;
      AgsChannel *old_source;
      AgsRecycling *new_start_region, *new_end_region;
      AgsRecycling *old_start_region, *old_end_region;

      source = (AgsChannel *) g_value_get_object(value);

      if(recall_channel_run->source == source)
	return;

      old_source = recall_channel_run->source;

      if(old_source != NULL){
	old_start_region = old_source->first_recycling;
	old_end_region = old_source->last_recycling;
      }else{
	old_start_region = NULL;
	old_end_region = NULL;
      }

      if(source != NULL){
	g_object_ref(G_OBJECT(source));

	new_start_region = source->first_recycling;
	new_end_region = source->last_recycling;
      }else{
	new_start_region = NULL;
	new_end_region = NULL;
      }

      recall_channel_run->source = source;

      if(source == recall_channel_run->destination)
	g_warning("destination == recall_channel_run->source\0");

      if(old_source != NULL)
	g_object_unref(G_OBJECT(old_source));
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
  case PROP_AUDIO_CHANNEL:
    {
      g_value_set_uint(value, recall_channel_run->audio_channel);
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
  case PROP_DESTINATION:
    {
      g_value_set_object(value, recall_channel_run->destination);
    }
    break;
  case PROP_SOURCE:
    {
      g_value_set_object(value, recall_channel_run->source);
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

  if(recall_channel_run->destination != NULL)
    g_object_unref(recall_channel_run->destination);

  if(recall_channel_run->source != NULL)
    g_object_unref(recall_channel_run->source);

  G_OBJECT_CLASS(ags_recall_channel_run_parent_class)->finalize(gobject);
}

void
ags_recall_channel_run_connect(AgsConnectable *connectable)
{
  AgsRecallChannelRun *recall_channel_run;
  GObject *gobject;

  ags_recall_channel_run_parent_connectable_interface->connect(connectable);

  /* AgsCopyChannelRun */
  recall_channel_run = AGS_RECALL_CHANNEL_RUN(connectable);

  /* destination */
  if(recall_channel_run->destination != NULL){
    gobject = G_OBJECT(recall_channel_run->destination);
    
    /* recycling changed */
    recall_channel_run->destination_recycling_changed_handler =
      g_signal_connect(gobject, "recycling_changed\0",
		       G_CALLBACK(ags_recall_channel_run_destination_recycling_changed_callback), recall_channel_run);
  }

  /* source */
  gobject = G_OBJECT(recall_channel_run->source);

  recall_channel_run->source_recycling_changed_handler =
    g_signal_connect(gobject, "recycling_changed\0",
		     G_CALLBACK(ags_recall_channel_run_source_recycling_changed_callback), recall_channel_run);
}

void
ags_recall_channel_run_disconnect(AgsConnectable *connectable)
{
  AgsRecallChannelRun *recall_channel_run;
  GObject *gobject;

  /* AgsRecallChannelRun */
  recall_channel_run = AGS_RECALL_CHANNEL_RUN(connectable);

  /* destination */
  if(recall_channel_run->destination != NULL &&
     (AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(recall_channel_run)->flags)) != 0){
    gobject = G_OBJECT(recall_channel_run->destination);
  
    g_signal_handler_disconnect(gobject, recall_channel_run->destination_recycling_changed_handler);
  }

  /* source */
  if(recall_channel_run->source != NULL &&
     (AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(recall_channel_run)->flags)) != 0){
    gobject = G_OBJECT(recall_channel_run->source);

    g_signal_handler_disconnect(gobject, recall_channel_run->source_recycling_changed_handler);
  }

  /* call parent */
  ags_recall_channel_run_parent_connectable_interface->disconnect(connectable);
}

gboolean
ags_recall_channel_run_pack(AgsPackable *packable, GObject *container)
{
  AgsRecallAudioRun *recall_audio_run;
  AgsRecallContainer *recall_container;
  GList *list;
  AgsRecallID *recall_id;

  if(ags_recall_channel_run_parent_packable_interface->pack(packable, container))
    return(TRUE);

  recall_container = AGS_RECALL_CONTAINER(container);

  /* set AgsRecallAudioRun */
  list = recall_container->recall_audio_run;

  if(AGS_RECALL(packable)->recall_id != NULL){
    recall_id = AGS_RECALL(packable)->recall_id;
      
    list = ags_recall_find_recycling_container(list,
					       recall_id->recycling_container);

    if(list != NULL){
      recall_audio_run = AGS_RECALL_AUDIO_RUN(list->data);

      g_object_set(G_OBJECT(packable),
		   "recall_audio_run\0", recall_audio_run,
		   NULL);
    }
  }else if((AGS_RECALL_TEMPLATE & (AGS_RECALL(packable)->flags)) != 0){
    list = ags_recall_find_template(list);

    if(list != NULL){
      recall_audio_run = AGS_RECALL_AUDIO_RUN(list->data);

      g_object_set(G_OBJECT(packable),
		   "recall_audio_run\0", recall_audio_run,
		   NULL);
    }
  }

  /* set AgsRecallChannel */
  if(AGS_RECALL_CHANNEL_RUN(packable)->source != NULL){
    list = recall_container->recall_channel;

    if((list = ags_recall_find_provider(list,
					G_OBJECT(AGS_RECALL_CHANNEL_RUN(packable)->source))) != NULL){
      g_object_set(G_OBJECT(packable),
		   "recall_channel\0", AGS_RECALL_CHANNEL(list->data),
		   NULL);
    }
  }

  g_object_set(G_OBJECT(recall_container),
	       "recall_channel_run\0", AGS_RECALL(packable),
	       NULL);

  return(FALSE);
}

gboolean
ags_recall_channel_run_unpack(AgsPackable *packable)
{
  AgsRecall *recall;
  AgsRecallContainer *recall_container;
  GList *list;
  AgsRecallID *recall_id;

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
ags_recall_channel_run_connect_dynamic(AgsConnectable *connectable)
{
  ags_recall_channel_run_parent_dynamic_connectable_interface->connect_dynamic(connectable);

  /* empty */
}

void
ags_recall_channel_run_disconnect_dynamic(AgsConnectable *connectable)
{
  ags_recall_channel_run_parent_dynamic_connectable_interface->disconnect_dynamic(connectable);

  /* empty */
}

AgsRecall*
ags_recall_channel_run_duplicate(AgsRecall *recall,
				 AgsRecallID *recall_id,
				 guint *n_params, GParameter *parameter)
{
  AgsAudio *audio;
  AgsChannel *output;
  AgsRecallChannelRun *recall_channel_run, *copy;
  AgsRecallID *output_recall_id;

  recall_channel_run = AGS_RECALL_CHANNEL_RUN(recall);

  parameter = ags_parameter_grow(G_OBJECT_TYPE(recall),
				 parameter, n_params,
				 "devout\0", AGS_RECALL(recall_channel_run)->devout,
				 "recall_channel\0", recall_channel_run->recall_channel,
				 //				 "audio_channel\0", recall_channel_run->audio_channel,
				 "source\0", recall_channel_run->source,
				 "destination\0", recall_channel_run->destination,
				 NULL);
  copy = AGS_RECALL_CHANNEL_RUN(AGS_RECALL_CLASS(ags_recall_channel_run_parent_class)->duplicate(recall,
												 recall_id,
												 n_params, parameter));

  audio = AGS_AUDIO(copy->source->audio);

  if(AGS_IS_OUTPUT(copy->source)){
    output = copy->source;
    output_recall_id = ags_recall_id_find_recycling_container(output->recall_id,
							      recall_id->recycling_container);
  }else{
    AgsRecallID *default_recall_id, *audio_recall_id;

    if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
      output = ags_channel_nth(audio->output,
			       copy->source->audio_channel);
    }else{
      output = ags_channel_nth(audio->output,
			       copy->source->line);
    }


    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      default_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
								 recall_id->recycling_container);

      audio_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							       default_recall_id->recycling_container);

      output_recall_id = ags_recall_id_find_recycling_container(output->recall_id,
								audio_recall_id->recycling_container->parent);
    }else{
      audio_recall_id = ags_recall_id_find_recycling_container(audio->recall_id,
							       recall_id->recycling_container);


      output_recall_id = ags_recall_id_find_recycling_container(output->recall_id,
								audio_recall_id->recycling_container);
    }
  }
  
  if(recall_id->recycling_container->parent != NULL){
    copy->run_order = g_list_index(recall_id->recycling_container->parent->children,
				   recall_id->recycling_container);
  }else if(copy->destination != NULL){
    copy->run_order = copy->destination->audio_channel;
  }else{
    AgsRecycling *recycling;

    recycling = AGS_RECYCLING(recall_id->recycling);

    copy->run_order = AGS_CHANNEL(recycling->channel)->audio_channel;
  }

  if(copy->destination != NULL){
    ags_recall_channel_run_remap_child_destination(copy,
						   NULL, NULL,
						   copy->destination->first_recycling, copy->destination->last_recycling);
  }else{
    ags_recall_channel_run_remap_child_source(copy,
					      NULL, NULL,
					      copy->source->first_recycling, copy->source->last_recycling);
  }

  return((AgsRecall *) copy);
}

void
ags_recall_channel_run_map_recall_recycling(AgsRecallChannelRun *recall_channel_run)
{
  AgsRecallChannel *recall_channel;
  AgsRecycling *source_recycling, *destination_recycling;

  if(recall_channel_run->source == NULL ||
     AGS_RECALL(recall_channel_run)->child_type == G_TYPE_NONE ||
     (AGS_RECALL_TEMPLATE & (AGS_RECALL(recall_channel_run)->flags)) != 0)
    return;

  /* AgsRecallChannel */
  recall_channel = AGS_RECALL_CHANNEL(recall_channel_run->recall_channel);

  /* AgsRecycling - source */
  source_recycling = recall_channel->source->first_recycling;

  /* AgsRecycling - destination*/
  if(recall_channel_run->destination != NULL){
    destination_recycling = recall_channel_run->destination->first_recycling;
  }else{
    destination_recycling = NULL;
  }

  if(source_recycling != NULL){
    AgsRecallRecycling *recall_recycling;

    while(source_recycling != recall_channel->source->last_recycling->next){
#ifdef AGS_DEBUG
      g_message("ags_recall_channel_run_map_recall_recycling\0");
#endif

      do{
	if(AGS_RECALL(recall_channel_run)->child_type != G_TYPE_NONE){
	  recall_recycling = g_object_new(AGS_RECALL(recall_channel_run)->child_type,
					  "devout\0", AGS_RECALL(recall_channel_run)->devout,
					  "recall_id\0", AGS_RECALL(recall_channel_run)->recall_id,
					  "audio_channel\0", recall_channel_run->audio_channel,
					  "destination\0", destination_recycling,
					  "source\0", source_recycling,
					  NULL);

	  ags_recall_add_child(AGS_RECALL(recall_channel_run), AGS_RECALL(recall_recycling));
	}

	if(destination_recycling != NULL){
	  destination_recycling = destination_recycling->next;
	}
      }while(destination_recycling != NULL);

      source_recycling = source_recycling->next;
    }
  }
}

void
ags_recall_channel_run_remap_child_source(AgsRecallChannelRun *recall_channel_run,
					  AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
					  AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region)
{
  AgsRecycling *destination_recycling, *source_recycling;
  AgsRecallRecycling *recall_recycling;
  GList *list;

  if(recall_channel_run->source == NULL ||
     AGS_RECALL(recall_channel_run)->child_type == G_TYPE_NONE ||
     (AGS_RECALL_TEMPLATE & (AGS_RECALL(recall_channel_run)->flags)) != 0)
    return;

  /* remove old */
  if(old_start_changed_region != NULL){
    AgsDevout *devout;
    AgsRecall *recall;
    AgsCancelRecall *cancel_recall;

    devout = AGS_DEVOUT(AGS_AUDIO(recall_channel_run->source->audio)->devout);
    source_recycling = old_start_changed_region;

    while(source_recycling != old_end_changed_region->next){
      list = ags_recall_get_children(AGS_RECALL(recall_channel_run));

      while(list != NULL){
	if(AGS_RECALL_RECYCLING(list->data)->source == source_recycling){
	  recall = AGS_RECALL(list->data);

	  recall->flags |= AGS_RECALL_HIDE;
	  cancel_recall = ags_cancel_recall_new(recall,
						NULL);

	  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(devout->ags_main)->main_loop)->task_thread),
				      (AgsTask *) cancel_recall);
	}

	list = list->next;
      }

      source_recycling = source_recycling->next;
    }
  }

  /* add new */
  if(new_start_changed_region != NULL){
    source_recycling = new_start_changed_region;
      
    while(source_recycling != new_end_changed_region->next){

      if(AGS_RECALL(recall_channel_run)->child_type != G_TYPE_NONE){
	recall_recycling = g_object_new(AGS_RECALL(recall_channel_run)->child_type,
					"devout\0", AGS_RECALL(recall_channel_run)->devout,
					"recall_id\0", AGS_RECALL(recall_channel_run)->recall_id,
					"audio_channel\0", recall_channel_run->audio_channel,
					"source\0", source_recycling,
					"destination\0", NULL,
					NULL);
	
	ags_recall_add_child(AGS_RECALL(recall_channel_run), AGS_RECALL(recall_recycling));
      }

      source_recycling = source_recycling->next;
    }
  }
}

void
ags_recall_channel_run_remap_child_destination(AgsRecallChannelRun *recall_channel_run,
					       AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
					       AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region)
{
  AgsRecycling *destination_recycling, *source_recycling;
  AgsRecallRecycling *recall_recycling;
  GList *list;

  if(recall_channel_run->source == NULL ||
     AGS_RECALL(recall_channel_run)->child_type == G_TYPE_NONE ||
     (AGS_RECALL_TEMPLATE & (AGS_RECALL(recall_channel_run)->flags)) != 0)
    return;

  /* remove old */
  if(old_start_changed_region != NULL){
    AgsDevout *devout;
    AgsRecall *recall;
    AgsCancelRecall *cancel_recall;

    devout = AGS_DEVOUT(AGS_AUDIO(recall_channel_run->source->audio)->devout);
    destination_recycling = old_start_changed_region;
    
    while(destination_recycling != old_end_changed_region->next){
      list = ags_recall_get_children(AGS_RECALL(recall_channel_run));

      while(list != NULL){
	if(AGS_RECALL_RECYCLING(list->data)->destination == destination_recycling){
	  recall = AGS_RECALL(list->data);

	  recall->flags |= AGS_RECALL_HIDE;
	  cancel_recall = ags_cancel_recall_new(recall,
						NULL);

	  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(devout->ags_main)->main_loop)->task_thread),
				      (AgsTask *) cancel_recall);
	}

	list = list->next;
      }

      destination_recycling = destination_recycling->next;
    }
  }

  /* add new */
  if(new_start_changed_region != NULL){
    if(recall_channel_run->source != NULL){
      if(recall_channel_run->source->first_recycling == NULL)
	return;
      
      destination_recycling = new_start_changed_region;
      
      while(destination_recycling != new_end_changed_region->next){
	source_recycling = recall_channel_run->source->first_recycling;
	
	while(source_recycling != recall_channel_run->source->last_recycling->next){
	  if(AGS_RECALL(recall_channel_run)->child_type != G_TYPE_NONE){
	    recall_recycling = g_object_new(AGS_RECALL(recall_channel_run)->child_type,
					    "devout\0", AGS_RECALL(recall_channel_run)->devout,
					    "recall_id\0", AGS_RECALL(recall_channel_run)->recall_id,
					    "audio_channel\0", recall_channel_run->audio_channel,
					    "source\0", source_recycling,
					    "destination\0", destination_recycling,
					    NULL);
	  
	    ags_recall_add_child(AGS_RECALL(recall_channel_run), AGS_RECALL(recall_recycling));
	  }

	  source_recycling = source_recycling->next;
	}
	
	destination_recycling = destination_recycling->next;
      }
    }
  }
}

void
ags_recall_channel_run_source_recycling_changed_callback(AgsChannel *channel,
							 AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							 AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							 AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
							 AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
							 AgsRecallChannelRun *recall_channel_run)
{
  /* empty */
}


void
ags_recall_channel_run_destination_recycling_changed_callback(AgsChannel *channel,
							      AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							      AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							      AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
							      AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
							      AgsRecallChannelRun *recall_channel_run)
{
  /* empty */
}

void
ags_recall_channel_run_real_run_order_changed(AgsRecallChannelRun *recall_channel_run,
					      guint run_order)
{
  recall_channel_run->run_order = run_order;
}

/**
 * ags_recall_channel_run_run_order_changed:
 * @recall_channel_run: an #AgsRecallChannelRun
 * @run_order: the run order
 *
 * Modify run order.
 *
 * Since: 0.4
 */
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

/**
 * ags_recall_channel_run_new:
 *
 * Creates an #AgsRecallChannelRun
 *
 * Returns: a new #AgsRecallChannelRun
 *
 * Since: 0.4
 */
AgsRecallChannelRun*
ags_recall_channel_run_new()
{
  AgsRecallChannelRun *recall_channel_run;

  recall_channel_run = (AgsRecallChannelRun *) g_object_new(AGS_TYPE_RECALL_CHANNEL_RUN, NULL);

  return(recall_channel_run);
}
