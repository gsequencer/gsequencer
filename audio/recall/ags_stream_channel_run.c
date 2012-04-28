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

#include <ags/audio/recall/ags_stream_channel_run.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/task/ags_cancel_recall.h>

#include <ags/audio/recall/ags_stream_recycling.h>

void ags_stream_channel_run_class_init(AgsStreamChannelRunClass *stream_channel_run);
void ags_stream_channel_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_stream_channel_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_stream_channel_run_init(AgsStreamChannelRun *stream_channel_run);
void ags_stream_channel_run_connect(AgsConnectable *connectable);
void ags_stream_channel_run_disconnect(AgsConnectable *connectable);
void ags_stream_channel_run_run_connect(AgsRunConnectable *run_connectable);
void ags_stream_channel_run_run_disconnect(AgsRunConnectable *run_connectable);
void ags_stream_channel_run_finalize(GObject *gobject);

AgsRecall* ags_stream_channel_run_duplicate(AgsRecall *recall,
					    AgsRecallID *recall_id,
					    guint *n_params, GParameter *parameter);

void ags_stream_channel_run_map_stream_recycling(AgsStreamChannelRun *stream_channel_run);
void ags_stream_channel_run_remap_stream_recycling(AgsStreamChannelRun *stream_channel_run,
						   AgsRecycling *old_start_region, AgsRecycling *old_end_region,
						   AgsRecycling *new_start_region, AgsRecycling *new_end_region);
void ags_stream_channel_run_refresh_stream_recycling(AgsStreamChannelRun *stream_channel_run,
						     AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
						     AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region);  

void ags_stream_channel_run_recycling_changed_callback(AgsChannel *channel,
						       AgsRecycling *old_start_region, AgsRecycling *old_end_region,
						       AgsRecycling *new_start_region, AgsRecycling *new_end_region,
						       AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
						       AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
						       AgsStreamChannelRun *stream_channel_run);

static gpointer ags_stream_channel_run_parent_class = NULL;
static AgsConnectableInterface *ags_stream_channel_run_parent_connectable_interface;
static AgsRunConnectableInterface *ags_stream_channel_run_parent_run_connectable_interface;

GType
ags_stream_channel_run_get_type()
{
  static GType ags_type_stream_channel_run = 0;

  if(!ags_type_stream_channel_run){
    static const GTypeInfo ags_stream_channel_run_info = {
      sizeof (AgsStreamChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stream_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStreamChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stream_channel_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stream_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stream_channel_run_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_stream_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
							 "AgsStreamChannelRun\0",
							 &ags_stream_channel_run_info,
							 0);

    g_type_add_interface_static(ags_type_stream_channel_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_stream_channel_run,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return (ags_type_stream_channel_run);
}

void
ags_stream_channel_run_class_init(AgsStreamChannelRunClass *stream_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_stream_channel_run_parent_class = g_type_class_peek_parent(stream_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) stream_channel_run;

  gobject->finalize = ags_stream_channel_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) stream_channel_run;

  recall->duplicate = ags_stream_channel_run_duplicate;
}

void
ags_stream_channel_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_stream_channel_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_stream_channel_run_connect;
  connectable->disconnect = ags_stream_channel_run_disconnect;
}

void
ags_stream_channel_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_stream_channel_run_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_stream_channel_run_run_connect;
  run_connectable->disconnect = ags_stream_channel_run_run_disconnect;
}

void
ags_stream_channel_run_init(AgsStreamChannelRun *stream_channel_run)
{
  /* empty */
}

void
ags_stream_channel_run_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_stream_channel_run_parent_class)->finalize(gobject);
}

void
ags_stream_channel_run_connect(AgsConnectable *connectable)
{
  ags_stream_channel_run_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_stream_channel_run_disconnect(AgsConnectable *connectable)
{
  ags_stream_channel_run_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_stream_channel_run_run_connect(AgsRunConnectable *run_connectable)
{
  AgsChannel *channel;
  AgsStreamChannelRun *stream_channel_run;

  ags_stream_channel_run_parent_run_connectable_interface->connect(run_connectable);

  stream_channel_run = AGS_STREAM_CHANNEL_RUN(run_connectable);

  channel = AGS_RECALL_CHANNEL_RUN(stream_channel_run)->recall_channel->channel;

  stream_channel_run->channel_recycling_changed_handler =
    g_signal_connect(G_OBJECT(channel), "recycling_changed\0",
		     G_CALLBACK(ags_stream_channel_run_recycling_changed_callback), stream_channel_run);
}

void
ags_stream_channel_run_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsChannel *channel;
  AgsStreamChannelRun *stream_channel_run;

  ags_stream_channel_run_parent_run_connectable_interface->disconnect(run_connectable);

  stream_channel_run = AGS_STREAM_CHANNEL_RUN(run_connectable);

  channel = AGS_RECALL_CHANNEL_RUN(stream_channel_run)->recall_channel->channel;

  g_signal_handler_disconnect(G_OBJECT(channel),
			      stream_channel_run->channel_recycling_changed_handler);
}

AgsRecall*
ags_stream_channel_run_duplicate(AgsRecall *recall,
				 AgsRecallID *recall_id,
				 guint *n_params, GParameter *parameter)
{
  AgsStreamChannelRun *stream_channel_run, *copy;

  stream_channel_run = (AgsStreamChannelRun *) recall;  
  copy = (AgsStreamChannelRun *) AGS_RECALL_CLASS(ags_stream_channel_run_parent_class)->duplicate(recall,
												  recall_id,
												  n_params, parameter);

  ags_stream_channel_run_map_stream_recycling(copy);

  return((AgsRecall *) copy);
}

void
ags_stream_channel_run_map_stream_recycling(AgsStreamChannelRun *stream_channel_run)
{
  AgsChannel *channel;
  AgsRecycling *recycling;
  AgsStreamRecycling *stream_recycling;
  guint audio_channel;
  
  channel = AGS_RECALL_CHANNEL_RUN(stream_channel_run)->source;
  recycling = channel->first_recycling;

  if(recycling == NULL)
    return;

  audio_channel = channel->audio_channel;

  while(recycling != channel->last_recycling->next){
    stream_recycling = ags_stream_recycling_new(recycling);

    if((AGS_RECALL_PLAYBACK & (AGS_RECALL(stream_channel_run)->flags)) != 0)
      AGS_RECALL(stream_recycling)->flags |= AGS_RECALL_PROPAGATE_DONE;

    ags_recall_add_child(AGS_RECALL(stream_channel_run), AGS_RECALL(stream_recycling));

    recycling = recycling->next;
  }
}

void
ags_stream_channel_run_remap_stream_recycling(AgsStreamChannelRun *stream_channel_run,
					      AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					      AgsRecycling *new_start_region, AgsRecycling *new_end_region)
{
  AgsChannel *channel;
  AgsRecycling *recycling;
  AgsStreamRecycling *stream_recycling;
  GList *list;
  guint audio_channel;

  channel = AGS_RECALL_CHANNEL_RUN(stream_channel_run)->recall_channel->channel;
  audio_channel = channel->audio_channel;
  
  /* remove old */
  if(old_start_region !=  NULL){
    AgsDevout *devout;
    AgsRecall *recall;
    AgsCancelRecall *cancel_recall;

    devout = AGS_DEVOUT(AGS_AUDIO(channel->audio)->devout);
    recycling = old_start_region;

    while(recycling != old_end_region->next){
      list = ags_recall_get_children(AGS_RECALL(stream_channel_run));
      
      while(list != NULL){
	if(AGS_RECALL_RECYCLING(list->data)->source == recycling){
	  recall = AGS_RECALL(list->data);

	  recall->flags |= AGS_RECALL_HIDE;
	  cancel_recall = ags_cancel_recall_new(recall, NULL);

	  ags_devout_append_task(devout, (AgsTask *) cancel_recall);
	}

	list = list->next;
      }

      recycling = recycling->next;
    }
  }

  /* add new */
  if(new_start_region != NULL){
    recycling = new_start_region;

    while(recycling != new_end_region->next){
      stream_recycling = ags_stream_recycling_new(recycling);
      
      ags_recall_add_child(AGS_RECALL(stream_channel_run), AGS_RECALL(stream_recycling));
      
      recycling = recycling->next;
    }
  }
}


void
ags_stream_channel_run_refresh_stream_recycling(AgsStreamChannelRun *stream_channel_run,
						AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
						AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region)
{
  //TODO:JK: implement this function
}

void
ags_stream_channel_run_recycling_changed_callback(AgsChannel *channel,
						  AgsRecycling *old_start_region, AgsRecycling *old_end_region,
						  AgsRecycling *new_start_region, AgsRecycling *new_end_region,
						  AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
						  AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
						  AgsStreamChannelRun *stream_channel_run)
{
  ags_stream_channel_run_refresh_stream_recycling(stream_channel_run,
						  old_start_changed_region, old_end_changed_region,
						  new_start_changed_region, new_end_changed_region);
}

AgsStreamChannelRun*
ags_stream_channel_run_new()
{
  AgsStreamChannelRun *stream_channel_run;

  stream_channel_run = (AgsStreamChannelRun *) g_object_new(AGS_TYPE_STREAM_CHANNEL_RUN,
							    NULL);
  
  return(stream_channel_run);
}
