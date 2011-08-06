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

#include <ags/audio/recall/ags_copy_channel.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/task/ags_cancel_recall.h>

#include <ags/audio/recall/ags_copy_recycling.h>

#include <stdlib.h>
#include <stdio.h>

void ags_copy_channel_class_init(AgsCopyChannelClass *copy_channel);
void ags_copy_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_copy_channel_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_copy_channel_init(AgsCopyChannel *copy_channel);
void ags_copy_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_copy_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_copy_channel_connect(AgsConnectable *connectable);
void ags_copy_channel_disconnect(AgsConnectable *connectable);
void ags_copy_channel_run_connect(AgsRunConnectable *run_connectable);
void ags_copy_channel_run_disconnect(AgsRunConnectable *run_connectable);
void ags_copy_channel_finalize(GObject *gobject);

void ags_copy_channel_run_init_pre(AgsRecall *recall);
void ags_copy_channel_done(AgsRecall *recall);
void ags_copy_channel_remove(AgsRecall *recall);
void ags_copy_channel_cancel(AgsRecall *recall);
AgsRecall* ags_copy_channel_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

void ags_copy_channel_map_copy_recycling(AgsCopyChannel *copy_channel);
void ags_copy_channel_remap_child_destination(AgsCopyChannel *copy_channel,
					      AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					      AgsRecycling *new_start_region, AgsRecycling *new_end_region);
void ags_copy_channel_remap_child_source(AgsCopyChannel *copy_channel,
					 AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					 AgsRecycling *new_start_region, AgsRecycling *new_end_region);
void ags_copy_channel_refresh_child_source(AgsCopyChannel *copy_channel,
					   AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
					   AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region);
void ags_copy_channel_refresh_child_destination(AgsCopyChannel *copy_channel,
						AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
						AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region);

void ags_copy_channel_source_recycling_changed_callback(AgsChannel *channel,
							AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
							AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
							AgsCopyChannel *copy_channel);

void ags_copy_channel_destination_recycling_changed_callback(AgsChannel *channel,
							     AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							     AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							     AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
							     AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
							     AgsCopyChannel *copy_channel);

enum{
  PROP_0,
  PROP_DESTINATION,
  PROP_SOURCE,
  PROP_DEVOUT,
};

static gpointer ags_copy_channel_parent_class = NULL;
static AgsConnectableInterface *ags_copy_channel_parent_connectable_interface;
static AgsRunConnectableInterface *ags_copy_channel_parent_run_connectable_interface;

GType
ags_copy_channel_get_type()
{
  static GType ags_type_copy_channel = 0;

  if(!ags_type_copy_channel){
    static const GTypeInfo ags_copy_channel_info = {
      sizeof (AgsCopyChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_copy_channel = g_type_register_static(AGS_TYPE_RECALL,
						   "AgsCopyChannel\0",
						   &ags_copy_channel_info,
						   0);

    g_type_add_interface_static(ags_type_copy_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_copy_channel,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_copy_channel);
}

void
ags_copy_channel_class_init(AgsCopyChannelClass *copy_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_copy_channel_parent_class = g_type_class_peek_parent(copy_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_channel;

  gobject->set_property = ags_copy_channel_set_property;
  gobject->get_property = ags_copy_channel_get_property;

  gobject->finalize = ags_copy_channel_finalize;

  /* properties */
  param_spec = g_param_spec_object("destination\0",
				   "destination of output\0",
				   "The destination AgsChannel where it will output to\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DESTINATION,
				  param_spec);

  param_spec = g_param_spec_object("source\0",
				   "source of input\0",
				   "The source AgsChannel where it will take the input from\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOURCE,
				  param_spec);

  param_spec = g_param_spec_object("devout\0",
				   "assigned devout\0",
				   "The devout this recall is assigned to\0",
				   AGS_TYPE_DEVOUT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) copy_channel;

  recall->run_init_pre = ags_copy_channel_run_init_pre;
  recall->done = ags_copy_channel_done;
  recall->remove = ags_copy_channel_remove;
  recall->cancel = ags_copy_channel_cancel;
  recall->duplicate = ags_copy_channel_duplicate;
}

void
ags_copy_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_copy_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_copy_channel_connect;
  connectable->disconnect = ags_copy_channel_disconnect;
}

void
ags_copy_channel_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_copy_channel_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_copy_channel_run_connect;
  run_connectable->disconnect = ags_copy_channel_run_disconnect;
}

void
ags_copy_channel_init(AgsCopyChannel *copy_channel)
{
  copy_channel->devout = NULL;

  copy_channel->destination = NULL;

  copy_channel->source = NULL;
}

void
ags_copy_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsCopyChannel *copy_channel;

  copy_channel = AGS_COPY_CHANNEL(gobject);

  switch(prop_id){
  case PROP_DESTINATION:
    {
      AgsChannel *destination;
      AgsChannel *old_destination;
      AgsRecycling *new_start_region, *new_end_region;
      AgsRecycling *old_start_region, *old_end_region;

      destination = (AgsChannel *) g_value_get_object(value);

      if(copy_channel->destination == destination)
	return;

      old_destination = copy_channel->destination;

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

      copy_channel->destination = destination;

      ags_copy_channel_remap_child_destination(copy_channel,
					       old_start_region, old_end_region,
					       new_start_region, new_end_region);

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

      if(copy_channel->source == source)
	return;

      old_source = copy_channel->source;

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

      copy_channel->source = source;

      ags_copy_channel_remap_child_source(copy_channel,
					  old_start_region, old_end_region,
					  new_start_region, new_end_region);

      if(old_source != NULL)
	g_object_unref(G_OBJECT(old_source));
    }
    break;
  case PROP_DEVOUT:
    {
      AgsDevout *devout;

      devout = (AgsDevout *) g_value_get_object(value);

      if(copy_channel->devout == devout)
	return;

      if(copy_channel->devout != NULL)
	g_object_unref(G_OBJECT(copy_channel->devout));

      if(devout != NULL)
	g_object_ref(G_OBJECT(devout));

      copy_channel->devout = devout;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsCopyChannel *copy_channel;

  copy_channel = AGS_COPY_CHANNEL(gobject);

  switch(prop_id){
  case PROP_DESTINATION:
    {
      g_value_set_object(value, copy_channel->destination);
    }
    break;
  case PROP_SOURCE:
    {
      g_value_set_object(value, copy_channel->source);
    }
    break;
  case PROP_DEVOUT:
    {
      g_value_set_object(value, copy_channel->devout);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_copy_channel_connect(AgsConnectable *connectable)
{
  AgsCopyChannel *copy_channel;

  ags_copy_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_copy_channel_disconnect(AgsConnectable *connectable)
{
  ags_copy_channel_parent_connectable_interface->disconnect(connectable);
}

void
ags_copy_channel_run_connect(AgsRunConnectable *run_connectable)
{
  AgsCopyChannel *copy_channel;
  GObject *gobject;

  ags_copy_channel_parent_run_connectable_interface->connect(run_connectable);

  /* AgsCopyChannel */
  copy_channel = AGS_COPY_CHANNEL(run_connectable);

  /* destination */
  gobject = (GObject *) copy_channel->destination;

  copy_channel->destination_recycling_changed_handler =
    g_signal_connect(gobject, "recycling_changed\0",
		     G_CALLBACK(ags_copy_channel_destination_recycling_changed_callback), copy_channel);

  /* source */
  gobject = (GObject *) copy_channel->source;

  copy_channel->source_recycling_changed_handler =
    g_signal_connect(gobject, "recycling_changed\0",
		     G_CALLBACK(ags_copy_channel_source_recycling_changed_callback), copy_channel);
}

void
ags_copy_channel_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsCopyChannel *copy_channel;
  GObject *gobject;

  ags_copy_channel_parent_run_connectable_interface->disconnect(run_connectable);

  /* AgsCopyChannel */
  copy_channel = AGS_COPY_CHANNEL(run_connectable);

  /* destination */
  gobject = G_OBJECT(copy_channel->destination);
  
  g_signal_handler_disconnect(gobject, copy_channel->destination_recycling_changed_handler);

  /* source */
  gobject = G_OBJECT(copy_channel->source);

  g_signal_handler_disconnect(gobject, copy_channel->source_recycling_changed_handler);
}

void
ags_copy_channel_finalize(GObject *gobject)
{
  AgsCopyChannel *copy_channel;

  copy_channel = AGS_COPY_CHANNEL(gobject);

  if(copy_channel->devout != NULL)
    g_object_unref(copy_channel->devout);

  if(copy_channel->destination != NULL)
    g_object_unref(copy_channel->destination);

  if(copy_channel->source != NULL)
    g_object_unref(copy_channel->source);

  /* call parent */
  G_OBJECT_CLASS(ags_copy_channel_parent_class)->finalize(gobject);
}

void
ags_copy_channel_run_init_pre(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_copy_channel_parent_class)->run_init_pre(recall);

  /* empty */
}

void
ags_copy_channel_done(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_copy_channel_parent_class)->done(recall);
  
  /* empty */
}

void
ags_copy_channel_cancel(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_copy_channel_parent_class)->cancel(recall);
  
  /* empty */
}

void 
ags_copy_channel_remove(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_copy_channel_parent_class)->remove(recall);
  
  /* empty */
}

AgsRecall*
ags_copy_channel_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsCopyChannel *copy_channel, *copy;
  GValue devout_value = {0,};
  GValue destination_value = {0,};
  GValue source_value = {0,};

  copy_channel = (AgsCopyChannel *) recall;
  copy = (AgsCopyChannel *) AGS_RECALL_CLASS(ags_copy_channel_parent_class)->duplicate(recall, recall_id);

  g_value_init(&devout_value, G_TYPE_OBJECT);
  g_value_set_object(&devout_value, G_OBJECT(copy_channel->devout));
  g_object_set_property(G_OBJECT(copy),
			"devout\0",
			&devout_value);
  g_value_unset(&devout_value);

  g_value_init(&destination_value, G_TYPE_OBJECT);
  g_value_set_object(&destination_value, G_OBJECT(copy_channel->destination));
  g_object_set_property(G_OBJECT(copy),
			"destination\0",
			&destination_value);
  g_value_unset(&destination_value);

  g_value_init(&source_value, G_TYPE_OBJECT);
  g_value_set_object(&source_value, G_OBJECT(copy_channel->source));
  g_object_set_property(G_OBJECT(copy),
			"source\0",
			&source_value);
  g_value_unset(&source_value);
		     
  //  ags_copy_channel_map_copy_recycling(copy);

  return((AgsRecall *) copy);
}

void
ags_copy_channel_map_copy_recycling(AgsCopyChannel *copy_channel)
{
  AgsRecycling *destination_recycling, *source_recycling;
  AgsCopyRecycling *copy_recycling;

  destination_recycling = copy_channel->destination->first_recycling;

  if(destination_recycling == NULL || copy_channel->source->first_recycling == NULL)
    return;

  while(destination_recycling != copy_channel->destination->last_recycling->next){
    source_recycling = copy_channel->source->first_recycling;

    while(source_recycling != copy_channel->source->last_recycling->next){
      copy_recycling = ags_copy_recycling_new(destination_recycling,
					      source_recycling,
					      copy_channel->devout);

      ags_recall_add_child(AGS_RECALL(copy_channel), AGS_RECALL(copy_recycling));

      source_recycling = source_recycling->next;
    }

    destination_recycling = destination_recycling->next;
  }
}

void
ags_copy_channel_remap_child_destination(AgsCopyChannel *copy_channel,
					 AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					 AgsRecycling *new_start_region, AgsRecycling *new_end_region)
{
  AgsRecycling *destination_recycling, *source_recycling;
  AgsCopyRecycling *copy_recycling;
  GList *list;

  if(copy_channel->source == NULL)
    return;

  /* remove old */
  if(old_start_region != NULL){
    AgsDevout *devout;
    AgsRecall *recall;
    AgsCancelRecall *cancel_recall;

    devout = AGS_DEVOUT(AGS_AUDIO(copy_channel->source->audio)->devout);
    destination_recycling = old_start_region;

    while(destination_recycling != old_end_region->next){
      list = ags_recall_get_children(AGS_RECALL(copy_channel));

      while(list != NULL){
	if(AGS_COPY_RECYCLING(list->data)->destination == destination_recycling){
	  recall = AGS_RECALL(list->data);

	  recall->flags |= AGS_RECALL_HIDE;
	  cancel_recall = ags_cancel_recall_new(recall,
						NULL);

	  ags_devout_append_task(devout, (AgsTask *) cancel_recall);
	}

	list = list->next;
      }

      destination_recycling = destination_recycling->next;
    }
  }

  /* add new */
  if(new_start_region != NULL){
    if(copy_channel->source->first_recycling == NULL)
      return;

    destination_recycling = new_start_region;

    while(destination_recycling != new_end_region->next){
      source_recycling = copy_channel->source->first_recycling;

      while(source_recycling != copy_channel->source->last_recycling->next){
	copy_recycling = ags_copy_recycling_new(destination_recycling,
						source_recycling,
						copy_channel->devout);

	ags_recall_add_child(AGS_RECALL(copy_channel), AGS_RECALL(copy_recycling));

	source_recycling = source_recycling->next;
      }
      
      destination_recycling = destination_recycling->next;
    }
  }
}

void
ags_copy_channel_remap_child_source(AgsCopyChannel *copy_channel,
				    AgsRecycling *old_start_region, AgsRecycling *old_end_region,
				    AgsRecycling *new_start_region, AgsRecycling *new_end_region)
{
  AgsRecycling *destination_recycling, *source_recycling;
  AgsCopyRecycling *copy_recycling;
  GList *list;

  if(copy_channel->source == NULL)
    return;

  /* remove old */
  if(old_start_region != NULL){
    AgsDevout *devout;
    AgsRecall *recall;
    AgsCancelRecall *cancel_recall;

    devout = AGS_DEVOUT(AGS_AUDIO(copy_channel->source->audio)->devout);
    source_recycling = old_start_region;

    while(source_recycling != old_end_region->next){
      list = ags_recall_get_children(AGS_RECALL(copy_channel));

      while(list != NULL){
	if(AGS_COPY_RECYCLING(list->data)->source == source_recycling){
	  recall = AGS_RECALL(list->data);

	  recall->flags |= AGS_RECALL_HIDE;
	  cancel_recall = ags_cancel_recall_new(recall,
						NULL);

	  ags_devout_append_task(devout, (AgsTask *) cancel_recall);
	}

	list = list->next;
      }

      source_recycling = source_recycling->next;
    }
  }

  /* add new */
  if(new_start_region != NULL){
    if(copy_channel->destination == NULL ||
       copy_channel->destination->first_recycling == NULL)
      return;

    destination_recycling = copy_channel->destination->first_recycling;

    while(destination_recycling != copy_channel->destination->last_recycling->next){
      source_recycling = new_start_region;

      while(source_recycling != new_end_region->next){
	printf("ags_copy_channel_remap_child_source - [destination,source]: %u %u\n\0",
	       AGS_CHANNEL(destination_recycling->channel)->line,
	       AGS_CHANNEL(source_recycling->channel)->line);
	copy_recycling = ags_copy_recycling_new(destination_recycling,
						source_recycling,
						copy_channel->devout);

	ags_recall_add_child(AGS_RECALL(copy_channel), AGS_RECALL(copy_recycling));

	source_recycling = source_recycling->next;
      }
      
      destination_recycling = destination_recycling->next;
    }
  }
}

void
ags_copy_channel_refresh_child_source(AgsCopyChannel *copy_channel,
				      AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
				      AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region)
{
  //TODO:JK: implement this function
}

void
ags_copy_channel_refresh_child_destination(AgsCopyChannel *copy_channel,
					   AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
					   AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region)
{
  //TODO:JK: implement this function
}

void
ags_copy_channel_source_recycling_changed_callback(AgsChannel *channel,
						   AgsRecycling *old_start_region, AgsRecycling *old_end_region,
						   AgsRecycling *new_start_region, AgsRecycling *new_end_region,
						   AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
						   AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
						   AgsCopyChannel *copy_channel)
{
  ags_copy_channel_refresh_child_source(copy_channel,
					old_start_changed_region, old_end_changed_region,
					new_start_changed_region, new_end_changed_region);
}

void
ags_copy_channel_destination_recycling_changed_callback(AgsChannel *channel,
							AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
							AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
							AgsCopyChannel *copy_channel)
{
  ags_copy_channel_refresh_child_destination(copy_channel,
					     old_start_changed_region, old_end_changed_region,
					     new_start_changed_region, new_end_changed_region);
}

AgsCopyChannel*
ags_copy_channel_new(AgsChannel *destination,
		     AgsChannel *source,
		     AgsDevout *devout)
{
  AgsCopyChannel *copy_channel;

  copy_channel = (AgsCopyChannel *) g_object_new(AGS_TYPE_COPY_CHANNEL,
						 "destination\0", destination,
						 "source\0", source,
						 "devout\0", devout,
						 NULL);

  return(copy_channel);
}
