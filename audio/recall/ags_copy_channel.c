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
void ags_copy_channel_connect(AgsConnectable *connectable);
void ags_copy_channel_disconnect(AgsConnectable *connectable);
void ags_copy_channel_run_connect(AgsRunConnectable *run_connectable);
void ags_copy_channel_run_disconnect(AgsRunConnectable *run_connectable);
void ags_copy_channel_finalize(GObject *gobject);

void ags_copy_channel_run_init_pre(AgsRecall *recall, guint audio_channel, gpointer data);

void ags_copy_channel_done(AgsRecall *recall, gpointer data);
void ags_copy_channel_remove(AgsRecall *recall, gpointer data);
void ags_copy_channel_cancel(AgsRecall *recall, gpointer data);

AgsRecall* ags_copy_channel_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

void ags_copy_channel_map_copy_recycling(AgsCopyChannel *copy_channel);
void ags_copy_channel_remap_child_destination(AgsCopyChannel *copy_channel,
					      AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					      AgsRecycling *new_start_region, AgsRecycling *new_end_region);
void ags_copy_channel_remap_child_source(AgsCopyChannel *copy_channel,
					 AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					 AgsRecycling *new_start_region, AgsRecycling *new_end_region);

void ags_copy_channel_source_recycling_changed_callback(AgsChannel *channel,
							AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							AgsCopyChannel *copy_channel);

void ags_copy_channel_destination_recycling_changed_callback(AgsChannel *channel,
							     AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							     AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							     AgsCopyChannel *copy_channel);

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

  ags_copy_channel_parent_class = g_type_class_peek_parent(copy_channel);

  gobject = (GObjectClass *) copy_channel;

  gobject->finalize = ags_copy_channel_finalize;

  recall = (AgsRecallClass *) copy_channel;

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
ags_copy_channel_connect(AgsConnectable *connectable)
{
  AgsCopyChannel *copy_channel;

  ags_copy_channel_parent_connectable_interface->connect(connectable);

  /* AgsCopyChannel */
  copy_channel = AGS_COPY_CHANNEL(connectable);

  g_signal_connect((GObject *) copy_channel, "run_init_pre\0",
		   G_CALLBACK(ags_copy_channel_run_init_pre), NULL);

  g_signal_connect((GObject *) copy_channel, "done\0",
		   G_CALLBACK(ags_copy_channel_done), NULL);

  g_signal_connect((GObject *) copy_channel, "remove\0",
		   G_CALLBACK(ags_copy_channel_remove), NULL);

  g_signal_connect((GObject *) copy_channel, "cancel\0",
		   G_CALLBACK(ags_copy_channel_cancel), NULL);
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
  G_OBJECT_CLASS(ags_copy_channel_parent_class)->finalize(gobject);
}

void
ags_copy_channel_run_init_pre(AgsRecall *recall, guint audio_channel, gpointer data)
{
  /* empty */
}

void
ags_copy_channel_done(AgsRecall *recall, gpointer data)
{
  AgsCopyChannel *copy_channel;

  copy_channel = AGS_COPY_CHANNEL(recall);

  //  ags_copy_channel_disconnect_run_handler(copy_channel);
}

void
ags_copy_channel_cancel(AgsRecall *recall, gpointer data)
{
  //  ags_copy_channel_disconnect_run_handler((AgsCopyChannel *) recall);
}

void 
ags_copy_channel_remove(AgsRecall *recall, gpointer data)
{
  /* empty */
}

AgsRecall*
ags_copy_channel_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsCopyChannel *copy_channel, *copy;

  copy_channel = (AgsCopyChannel *) recall;
  copy = (AgsCopyChannel *) AGS_RECALL_CLASS(ags_copy_channel_parent_class)->duplicate(recall, recall_id);

  copy->devout = copy_channel->devout;
  copy->destination = copy_channel->destination;
  copy->source = copy_channel->source;

  ags_copy_channel_map_copy_recycling(copy);

  return((AgsRecall *) copy);
}

void
ags_copy_channel_map_copy_recycling(AgsCopyChannel *copy_channel)
{
  AgsRecycling *destination_recycling, *source_recycling;
  AgsCopyRecycling *copy_recycling;
  guint audio_channel;

  destination_recycling = copy_channel->destination->first_recycling;

  if(destination_recycling == NULL || copy_channel->source->first_recycling == NULL)
    return;

  audio_channel = copy_channel->source->audio_channel;

  while(destination_recycling != copy_channel->destination->last_recycling->next){
    source_recycling = copy_channel->source->first_recycling;

    while(source_recycling != copy_channel->source->last_recycling->next){
      copy_recycling = ags_copy_recycling_new(destination_recycling,
					      source_recycling,
					      copy_channel->devout);

      ags_recall_add_child(AGS_RECALL(copy_channel), AGS_RECALL(copy_recycling), audio_channel);

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
  guint audio_channel;

  audio_channel = copy_channel->source->audio_channel;

  /* remove old */
  if(old_start_region != NULL){
    AgsDevout *devout;
    AgsRecall *recall;
    AgsCancelRecall *cancel_recall;

    devout = AGS_DEVOUT(AGS_AUDIO(copy_channel->source->audio)->devout);
    destination_recycling = old_start_region;

    while(destination_recycling != old_end_region->next){
      list = copy_channel->recall.child;

      while(list != NULL){
	if(AGS_COPY_RECYCLING(list->data)->destination == destination_recycling){
	  recall = AGS_RECALL(list->data);

	  recall->flags |= AGS_RECALL_HIDE;
	  cancel_recall = ags_cancel_recall_new(recall, audio_channel,
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

	ags_recall_add_child(AGS_RECALL(copy_channel), AGS_RECALL(copy_recycling), audio_channel);

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
  guint audio_channel;

  audio_channel = copy_channel->source->audio_channel;

  /* remove old */
  if(old_start_region != NULL){
    AgsDevout *devout;
    AgsRecall *recall;
    AgsCancelRecall *cancel_recall;

    devout = AGS_DEVOUT(AGS_AUDIO(copy_channel->source->audio)->devout);
    source_recycling = old_start_region;

    while(source_recycling != old_end_region->next){
      list = copy_channel->recall.child;

      while(list != NULL){
	if(AGS_COPY_RECYCLING(list->data)->source == source_recycling){
	  recall = AGS_RECALL(list->data);

	  recall->flags |= AGS_RECALL_HIDE;
	  cancel_recall = ags_cancel_recall_new(recall, audio_channel,
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
    if(copy_channel->destination->first_recycling == NULL)
      return;

    destination_recycling = copy_channel->destination->first_recycling;

    while(destination_recycling != copy_channel->destination->last_recycling->next){
      source_recycling = new_start_region;

      while(source_recycling != new_end_region->next){
	copy_recycling = ags_copy_recycling_new(destination_recycling,
						source_recycling,
						copy_channel->devout);

	ags_recall_add_child(AGS_RECALL(copy_channel), AGS_RECALL(copy_recycling), audio_channel);

	source_recycling = source_recycling->next;
      }
      
      destination_recycling = destination_recycling->next;
    }
  }
}

void
ags_copy_channel_source_recycling_changed_callback(AgsChannel *channel,
						   AgsRecycling *old_start_region, AgsRecycling *old_end_region,
						   AgsRecycling *new_start_region, AgsRecycling *new_end_region,
						   AgsCopyChannel *copy_channel)
{
  ags_copy_channel_remap_child_source(copy_channel,
				      old_start_region, old_end_region,
				      new_start_region, new_end_region);
}

void
ags_copy_channel_destination_recycling_changed_callback(AgsChannel *channel,
							AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							AgsCopyChannel *copy_channel)
{
  ags_copy_channel_remap_child_destination(copy_channel,
					   old_start_region, old_end_region,
					   new_start_region, new_end_region);
}

AgsCopyChannel*
ags_copy_channel_new(AgsChannel *destination,
		     AgsChannel *source,
		     AgsDevout *devout)
{
  AgsCopyChannel *copy_channel;

  copy_channel = (AgsCopyChannel *) g_object_new(AGS_TYPE_COPY_CHANNEL, NULL);

  copy_channel->destination = destination;

  copy_channel->source = source;

  copy_channel->devout = devout;

  return(copy_channel);
}
