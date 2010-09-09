#include "ags_play_channel.h" 

#include "../ags_devout.h"
#include "../ags_audio.h"
#include "../ags_recycling.h"
#include "../ags_audio_signal.h"
#include "../ags_recall_id.h"

#include "ags_play_recycling.h"

#include <stdlib.h>
#include <stdio.h>

GType ags_play_channel_get_type();
void ags_play_channel_class_init(AgsPlayChannelClass *play_channel);
void ags_play_channel_init(AgsPlayChannel *play_channel);
void ags_play_channel_finalize(GObject *gobject);

void ags_play_channel_run_init_pre(AgsPlayChannel *play_channel, gpointer data);

void ags_play_channel_run_inter(AgsRecall *recall, gpointer data);

void ags_play_channel_done(AgsRecall *recall, gpointer data);
void ags_play_channel_remove(AgsRecall *recall, gpointer data);
void ags_play_channel_cancel(AgsRecall *recall, gpointer data);
AgsRecall* ags_play_channel_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

void ags_play_channel_map_play_recycling(AgsPlayChannel *play_channel);
void ags_play_channel_remap_child_source(AgsPlayChannel *play_channel,
					 AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					 AgsRecycling *new_start_region, AgsRecycling *new_end_region);


void ags_play_channel_source_recycling_changed_callback(AgsChannel *channel,
							AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							AgsPlayChannel *play_channel);

static gpointer ags_play_channel_parent_class = NULL;

GType
ags_play_channel_get_type()
{
  static GType ags_type_play_channel = 0;

  if(!ags_type_play_channel){
    static const GTypeInfo ags_play_channel_info = {
      sizeof (AgsPlayChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_channel_init,
    };

    ags_type_play_channel = g_type_register_static(AGS_TYPE_RECALL,
						   "AgsPlayChannel\0",
						   &ags_play_channel_info,
						   0);
  }

  return(ags_type_play_channel);
}

void
ags_play_channel_class_init(AgsPlayChannelClass *play_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_play_channel_parent_class = g_type_class_peek_parent(play_channel);

  gobject = (GObjectClass *) play_channel;

  gobject->finalize = ags_play_channel_finalize;

  recall = (AgsRecallClass *) play_channel;

  recall->duplicate = ags_play_channel_duplicate;
}

void
ags_play_channel_init(AgsPlayChannel *play_channel)
{
  //  play_channel->ref = 1;

  play_channel->devout = NULL;

  play_channel->source = NULL;
}

void
ags_play_channel_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_play_channel_parent_class)->finalize(gobject);
}

void
ags_play_channel_connect(AgsPlayChannel *play_channel)
{
  ags_recall_connect((AgsRecall *) play_channel);

  g_signal_connect((GObject *) play_channel, "run_init_pre\0",
		   G_CALLBACK(ags_play_channel_run_init_pre), NULL);

  g_signal_connect((GObject *) play_channel, "run_init_inter\0",
		   G_CALLBACK(ags_play_channel_run_init_pre), NULL);

  g_signal_connect((GObject *) play_channel, "run_inter\0",
		   G_CALLBACK(ags_play_channel_run_inter), NULL);

  g_signal_connect((GObject *) play_channel, "done\0",
		   G_CALLBACK(ags_play_channel_done), NULL);

  g_signal_connect((GObject *) play_channel, "remove\0",
		   G_CALLBACK(ags_play_channel_remove), NULL);

  g_signal_connect((GObject *) play_channel, "cancel\0",
		   G_CALLBACK(ags_play_channel_cancel), NULL);
}

void
ags_play_channel_connect_run_handler(AgsPlayChannel *play_channel)
{
  GObject *gobject;

  /* source */
  gobject = G_OBJECT(play_channel->source);

  play_channel->source_recycling_changed_handler =
    g_signal_connect(gobject, "recycling_changed\0",
		     G_CALLBACK(ags_play_channel_source_recycling_changed_callback), play_channel);
}

void
ags_play_channel_disconnect_run_handler(AgsPlayChannel *play_channel)
{
  GObject *gobject;

  gobject = G_OBJECT(play_channel->source);

  g_signal_handler_disconnect(gobject, play_channel->source_recycling_changed_handler);
}

void
ags_play_channel_run_init_pre(AgsPlayChannel *play_channel, gpointer data)
{
  ags_play_channel_map_play_recycling(play_channel);
}

void
ags_play_channel_run_init_inter(AgsPlayChannel *play_channel, gpointer data)
{
  ags_play_channel_connect_run_handler(play_channel);
}

void
ags_play_channel_run_inter(AgsRecall *recall, gpointer data)
{
  /* empty */
}

void
ags_play_channel_done(AgsRecall *recall, gpointer data)
{
  AgsPlayChannel *play_channel;

  play_channel = AGS_PLAY_CHANNEL(recall);

  ags_play_channel_disconnect_run_handler(play_channel);
}

void 
ags_play_channel_cancel(AgsRecall *recall, gpointer data)
{
  AgsPlayChannel *play_channel;

  play_channel = AGS_PLAY_CHANNEL(recall);

  ags_play_channel_disconnect_run_handler(play_channel);
}

void
ags_play_channel_remove(AgsRecall *recall, gpointer data)
{
  /* empty */
}

AgsRecall*
ags_play_channel_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsPlayChannel *play_channel, *copy;
  
  play_channel = (AgsPlayChannel *) recall;
  copy = (AgsPlayChannel *) AGS_RECALL_CLASS(ags_play_channel_parent_class)->duplicate(recall, recall_id);

  copy->devout = play_channel->devout;
  copy->source = play_channel->source;

  return((AgsRecall *) copy);
}

void
ags_play_channel_map_play_recycling(AgsPlayChannel *play_channel)
{
  AgsRecycling *source_recycling;
  AgsPlayRecycling *play_recycling;

  source_recycling = play_channel->source->first_recycling;

  if(source_recycling != NULL){
    while(source_recycling != play_channel->source->last_recycling->next){
      play_recycling = ags_play_recycling_new(source_recycling, play_channel->source->audio_channel,
					      play_channel->devout);

      play_recycling->recall.parent = (GObject *) play_channel;

      ags_play_recycling_connect(play_recycling);

      play_channel->recall.child = g_list_prepend(play_channel->recall.child, play_recycling);

      source_recycling = source_recycling->next;
    }
  }
}

void ags_play_channel_remap_child_source(AgsPlayChannel *play_channel,
					 AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					 AgsRecycling *new_start_region, AgsRecycling *new_end_region)
{
  AgsRecycling *source_recycling;
  AgsPlayRecycling *play_recycling;
  GList *list;

  /* remove old */
  if(old_start_region != NULL){
    source_recycling = old_start_region;

    while(source_recycling != old_end_region->next){
      list = play_channel->recall.child;

      while(list != NULL){
	if(AGS_PLAY_RECYCLING(list->data)->source == source_recycling)
	  AGS_RECALL(list->data)->flags |= AGS_RECALL_HIDE | AGS_RECALL_CANCEL;

	list = list->next;
      }

      source_recycling = source_recycling->next;
    }
  }

  /* remove new */
  if(new_start_region != NULL){
    source_recycling = new_start_region;

    while(source_recycling != new_end_region->next){
      play_recycling = ags_play_recycling_new(source_recycling, play_channel->source->audio_channel,
					      play_channel->devout);

      play_recycling->recall.parent = (GObject *) play_channel;

      ags_play_recycling_connect(play_recycling);

      play_channel->recall.child = g_list_prepend(play_channel->recall.child, play_recycling);

      if((AGS_RECALL_RUN_INITIALIZED & (play_channel->recall.flags)) != 0){
	ags_recall_run_init_pre((AgsRecall *) play_recycling);
	ags_recall_run_init_inter((AgsRecall *) play_recycling);
	ags_recall_run_init_post((AgsRecall *) play_recycling);

	play_recycling->recall.flags |= AGS_RECALL_RUN_INITIALIZED;
      }
      
      source_recycling = source_recycling->next;
    }
  }
}

void
ags_play_channel_source_recycling_changed_callback(AgsChannel *channel,
						   AgsRecycling *old_start_region, AgsRecycling *old_end_region,
						   AgsRecycling *new_start_region, AgsRecycling *new_end_region,
						   AgsPlayChannel *play_channel)
{
  ags_play_channel_remap_child_source(play_channel,
				      old_start_region, old_end_region,
				      new_start_region, new_end_region);
}

AgsPlayChannel*
ags_play_channel_new(AgsChannel *source,
		     AgsDevout *devout)
{
  AgsPlayChannel *play_channel;

  play_channel = (AgsPlayChannel *) g_object_new(AGS_TYPE_PLAY_CHANNEL, NULL);

  play_channel->devout = devout;

  play_channel->source = source;

  return(play_channel);
}
