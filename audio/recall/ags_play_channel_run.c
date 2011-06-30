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

#include <ags/audio/recall/ags_play_channel_run.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/task/ags_cancel_recall.h>

#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_recycling.h>

#include <stdlib.h>
#include <stdio.h>

void ags_play_channel_run_class_init(AgsPlayChannelRunClass *play_channel_run);
void ags_play_channel_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_channel_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_play_channel_run_init(AgsPlayChannelRun *play_channel_run);
void ags_play_channel_run_connect(AgsConnectable *connectable);
void ags_play_channel_run_disconnect(AgsConnectable *connectable);
void ags_play_channel_run_run_connect(AgsRunConnectable *run_connectable);
void ags_play_channel_run_run_disconnect(AgsRunConnectable *run_connectable);
void ags_play_channel_run_finalize(GObject *gobject);

void ags_play_channel_run_run_init_pre(AgsRecall *recall);
void ags_play_channel_run_done(AgsRecall *recall);
void ags_play_channel_run_remove(AgsRecall *recall);
void ags_play_channel_run_cancel(AgsRecall *recall);
AgsRecall* ags_play_channel_run_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

void ags_play_channel_run_map_play_recycling(AgsPlayChannelRun *play_channel_run);
void ags_play_channel_run_remap_child_source(AgsPlayChannelRun *play_channel_run,
					 AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					 AgsRecycling *new_start_region, AgsRecycling *new_end_region);


void ags_play_channel_run_source_recycling_changed_callback(AgsChannel *channel,
							AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							AgsPlayChannelRun *play_channel_run);

static gpointer ags_play_channel_run_parent_class = NULL;
static AgsConnectableInterface *ags_play_channel_run_parent_connectable_interface;
static AgsRunConnectableInterface *ags_play_channel_run_parent_run_connectable_interface;

GType
ags_play_channel_run_get_type()
{
  static GType ags_type_play_channel_run = 0;

  if(!ags_type_play_channel_run){
    static const GTypeInfo ags_play_channel_run_info = {
      sizeof (AgsPlayChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_channel_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_channel_run_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
						       "AgsPlayChannelRun\0",
						       &ags_play_channel_run_info,
						       0);

    g_type_add_interface_static(ags_type_play_channel_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_channel_run,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_play_channel_run);
}

void
ags_play_channel_run_class_init(AgsPlayChannelRunClass *play_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_play_channel_run_parent_class = g_type_class_peek_parent(play_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) play_channel_run;

  gobject->finalize = ags_play_channel_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_channel_run;

  recall->run_init_pre = ags_play_channel_run_run_init_pre;
  recall->done = ags_play_channel_run_done;
  recall->remove = ags_play_channel_run_remove;
  recall->cancel = ags_play_channel_run_cancel;
  recall->duplicate = ags_play_channel_run_duplicate;
}

void
ags_play_channel_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_channel_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_channel_run_connect;
  connectable->disconnect = ags_play_channel_run_disconnect;
}

void
ags_play_channel_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_play_channel_run_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_play_channel_run_run_connect;
  run_connectable->disconnect = ags_play_channel_run_run_disconnect;
}

void
ags_play_channel_run_init(AgsPlayChannelRun *play_channel_run)
{
  /* empty */
}

void
ags_play_channel_run_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_play_channel_run_parent_class)->finalize(gobject);
}

void
ags_play_channel_run_connect(AgsConnectable *connectable)
{
  AgsPlayChannelRun *play_channel_run;

  ags_play_channel_run_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_play_channel_run_disconnect(AgsConnectable *connectable)
{
  ags_play_channel_run_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_play_channel_run_run_connect(AgsRunConnectable *run_connectable)
{
  AgsPlayChannel *play_channel;
  AgsPlayChannelRun *play_channel_run;
  GObject *gobject;

  ags_play_channel_run_parent_run_connectable_interface->connect(run_connectable);

  /* AgsPlayChannelRun */
  play_channel_run = AGS_PLAY_CHANNEL_RUN(run_connectable);

  /* AgsPlayChannel */
  play_channel = AGS_PLAY_CHANNEL(AGS_RECALL_CHANNEL_RUN(play_channel_run)->recall_channel);

  /* source */
  gobject = G_OBJECT(AGS_RECALL_CHANNEL(play_channel)->channel);

  play_channel_run->source_recycling_changed_handler =
    g_signal_connect(gobject, "recycling_changed\0",
		     G_CALLBACK(ags_play_channel_run_source_recycling_changed_callback), play_channel_run);
}

void
ags_play_channel_run_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsPlayChannel *play_channel;
  AgsPlayChannelRun *play_channel_run;
  GObject *gobject;

  ags_play_channel_run_parent_run_connectable_interface->disconnect(run_connectable);

  /* AgsPlayChannelRun */
  play_channel_run = AGS_PLAY_CHANNEL_RUN(run_connectable);

  /* AgsPlayChannel */
  play_channel = AGS_PLAY_CHANNEL(AGS_RECALL_CHANNEL_RUN(play_channel_run)->recall_channel);

  /* source */
  gobject = G_OBJECT(AGS_RECALL_CHANNEL(play_channel)->channel);

  g_signal_handler_disconnect(gobject, play_channel_run->source_recycling_changed_handler);
}

void
ags_play_channel_run_run_init_pre(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_play_channel_run_parent_class)->run_init_pre(recall);

  /* empty */
}

void
ags_play_channel_run_done(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_play_channel_run_parent_class)->done(recall);

  /* empty */
}

void 
ags_play_channel_run_cancel(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_play_channel_run_parent_class)->cancel(recall);

  /* empty */
}

void
ags_play_channel_run_remove(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_play_channel_run_parent_class)->remove(recall);

  /* empty */
}

AgsRecall*
ags_play_channel_run_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsPlayChannelRun *play_channel_run, *copy;
  
  play_channel_run = (AgsPlayChannelRun *) recall;
  copy = (AgsPlayChannelRun *) AGS_RECALL_CLASS(ags_play_channel_run_parent_class)->duplicate(recall, recall_id);

  ags_play_channel_run_map_play_recycling(copy);

  return((AgsRecall *) copy);
}

void
ags_play_channel_run_map_play_recycling(AgsPlayChannelRun *play_channel_run)
{
  AgsPlayChannel *play_channel;
  AgsRecycling *source_recycling;

  /* AgsPlayChannel */
  play_channel = AGS_PLAY_CHANNEL(AGS_RECALL_CHANNEL_RUN(play_channel_run)->recall_channel);

  /* AgsRecycling */
  source_recycling = AGS_RECALL_CHANNEL(play_channel)->channel->first_recycling;

  if(source_recycling != NULL){
    AgsPlayRecycling *play_recycling;
    guint audio_channel;

    audio_channel = play_channel->audio_channel;

    while(source_recycling != AGS_RECALL_CHANNEL(play_channel)->channel->last_recycling->next){
      printf("ags_play_channel_run_map_play_recycling\n\0");

      play_recycling = ags_play_recycling_new(source_recycling,
					      play_channel->devout,
					      audio_channel);

      ags_recall_add_child(AGS_RECALL(play_channel_run), AGS_RECALL(play_recycling));

      source_recycling = source_recycling->next;
    }
  }
}

void ags_play_channel_run_remap_child_source(AgsPlayChannelRun *play_channel_run,
					     AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					     AgsRecycling *new_start_region, AgsRecycling *new_end_region)
{
  AgsPlayChannel *play_channel;
  AgsRecycling *source_recycling;
  AgsPlayRecycling *play_recycling;
  GList *list;
  guint audio_channel;

  /* AgsPlayChannel */
  play_channel = AGS_PLAY_CHANNEL(AGS_RECALL_CHANNEL_RUN(play_channel_run)->recall_channel);

  audio_channel = play_channel->audio_channel;

  /* remove old */
  if(old_start_region != NULL){
    AgsDevout *devout;
    AgsRecall *recall;
    AgsCancelRecall *cancel_recall;

    devout = AGS_DEVOUT(AGS_AUDIO(AGS_RECALL_CHANNEL(play_channel)->channel->audio)->devout);
    source_recycling = old_start_region;

    while(source_recycling != old_end_region->next){
      list = ags_recall_get_children(AGS_RECALL(play_channel_run));

      while(list != NULL){
	if(AGS_PLAY_RECYCLING(list->data)->source == source_recycling){
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

  /* remove new */
  if(new_start_region != NULL){
    source_recycling = new_start_region;
    
    while(source_recycling != new_end_region->next){
      play_recycling = ags_play_recycling_new(source_recycling,
					      play_channel->devout,
					      audio_channel);
      
      ags_recall_add_child(AGS_RECALL(play_channel_run), AGS_RECALL(play_recycling));
      
      source_recycling = source_recycling->next;
    }
  }
}

void
ags_play_channel_run_source_recycling_changed_callback(AgsChannel *channel,
						       AgsRecycling *old_start_region, AgsRecycling *old_end_region,
						       AgsRecycling *new_start_region, AgsRecycling *new_end_region,
						       AgsPlayChannelRun *play_channel_run)
{
  ags_play_channel_run_remap_child_source(play_channel_run,
					  old_start_region, old_end_region,
					  new_start_region, new_end_region);
}

AgsPlayChannelRun*
ags_play_channel_run_new()
{
  AgsPlayChannelRun *play_channel_run;

  play_channel_run = (AgsPlayChannelRun *) g_object_new(AGS_TYPE_PLAY_CHANNEL_RUN,
							NULL);
  
  return(play_channel_run);
}
